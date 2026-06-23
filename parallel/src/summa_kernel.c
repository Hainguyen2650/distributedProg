#include "summa_kernel.h"

#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    MPI_Comm grid_comm;
    MPI_Comm row_comm;
    MPI_Comm col_comm;
    int world_rank;
    int world_size;
    int grid_rank;
    int grid_size;
    int row;
    int col;
    int block_size;
    int block_elems;
} GridContext;

static void *checked_calloc(size_t count, size_t size) {
    void *ptr = calloc(count, size);
    if (!ptr) {
        fprintf(stderr, "Out of memory\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    return ptr;
}

int process_count_is_square(int process_count, int *grid_size_out) {
    int q = (int)(sqrt((double)process_count) + 0.5);
    if (grid_size_out) {
        *grid_size_out = q;
    }
    return q * q == process_count;
}

static int grid_create(MPI_Comm world, int n, GridContext *ctx) {
    MPI_Comm_rank(world, &ctx->world_rank);
    MPI_Comm_size(world, &ctx->world_size);

    if (!process_count_is_square(ctx->world_size, &ctx->grid_size)) {
        if (ctx->world_rank == 0) {
            fprintf(stderr, "Process count must be a perfect square for q x q SUMMA.\n");
            fprintf(stderr, "Examples: -np 1, -np 4, -np 9, -np 16.\n");
        }
        return 0;
    }

    if (n % ctx->grid_size != 0) {
        if (ctx->world_rank == 0) {
            fprintf(stderr, "N must be divisible by sqrt(P). N=%d, sqrt(P)=%d.\n",
                    n, ctx->grid_size);
        }
        return 0;
    }

    int dims[2] = {ctx->grid_size, ctx->grid_size};
    int periods[2] = {0, 0};
    MPI_Cart_create(world, 2, dims, periods, 1, &ctx->grid_comm);
    MPI_Comm_rank(ctx->grid_comm, &ctx->grid_rank);

    int coords[2] = {0, 0};
    MPI_Cart_coords(ctx->grid_comm, ctx->grid_rank, 2, coords);
    ctx->row = coords[0];
    ctx->col = coords[1];

    MPI_Comm_split(ctx->grid_comm, ctx->row, ctx->col, &ctx->row_comm);
    MPI_Comm_split(ctx->grid_comm, ctx->col, ctx->row, &ctx->col_comm);

    ctx->block_size = n / ctx->grid_size;
    ctx->block_elems = ctx->block_size * ctx->block_size;
    return 1;
}

static void grid_destroy(GridContext *ctx) {
    MPI_Comm_free(&ctx->row_comm);
    MPI_Comm_free(&ctx->col_comm);
    MPI_Comm_free(&ctx->grid_comm);
}

static void distribute_blocks(const AppConfig *config, const GridContext *ctx,
                              double *local_a, double *local_b, PhaseTiming *timing) {
    double start = timer_now();
    int bytes_per_block = ctx->block_elems * (int)sizeof(double);

    if (ctx->grid_rank == 0) {
        Matrix global_a = matrix_create(config->n, config->n);
        Matrix global_b = matrix_create(config->n, config->n);
        matrix_init(&global_a, config->init_mode, config->seed, 0);
        matrix_init(&global_b, config->init_mode, config->seed, 1);

        for (int r = 0; r < ctx->world_size; r++) {
            int coords[2];
            MPI_Cart_coords(ctx->grid_comm, r, 2, coords);

            double *tmp_a = checked_calloc((size_t)ctx->block_elems, sizeof(double));
            double *tmp_b = checked_calloc((size_t)ctx->block_elems, sizeof(double));
            matrix_copy_block(tmp_a, &global_a, ctx->block_size, ctx->block_size, coords[0], coords[1]);
            matrix_copy_block(tmp_b, &global_b, ctx->block_size, ctx->block_size, coords[0], coords[1]);

            if (r == 0) {
                for (int i = 0; i < ctx->block_elems; i++) {
                    local_a[i] = tmp_a[i];
                    local_b[i] = tmp_b[i];
                }
            } else {
                MPI_Send(tmp_a, ctx->block_elems, MPI_DOUBLE, r, 10, ctx->grid_comm);
                MPI_Send(tmp_b, ctx->block_elems, MPI_DOUBLE, r, 11, ctx->grid_comm);
                timing->bytes_sent += 2LL * bytes_per_block;
            }

            free(tmp_a);
            free(tmp_b);
        }

        matrix_destroy(&global_a);
        matrix_destroy(&global_b);
    } else {
        MPI_Recv(local_a, ctx->block_elems, MPI_DOUBLE, 0, 10, ctx->grid_comm, MPI_STATUS_IGNORE);
        MPI_Recv(local_b, ctx->block_elems, MPI_DOUBLE, 0, 11, ctx->grid_comm, MPI_STATUS_IGNORE);
        timing->bytes_received += 2LL * bytes_per_block;
    }

    timing->distribution += timer_now() - start;
}

static void run_summa_loop(const GridContext *ctx, double *local_a, double *local_b,
                           double *local_c, PhaseTiming *timing) {
    double *a_panel = checked_calloc((size_t)ctx->block_elems, sizeof(double));
    double *b_panel = checked_calloc((size_t)ctx->block_elems, sizeof(double));
    int bytes_per_block = ctx->block_elems * (int)sizeof(double);

    for (int k = 0; k < ctx->grid_size; k++) {
        if (ctx->col == k) {
            for (int i = 0; i < ctx->block_elems; i++) {
                a_panel[i] = local_a[i];
            }
        }
        if (ctx->row == k) {
            for (int i = 0; i < ctx->block_elems; i++) {
                b_panel[i] = local_b[i];
            }
        }

        double comm_start = timer_now();
        MPI_Bcast(a_panel, ctx->block_elems, MPI_DOUBLE, k, ctx->row_comm);
        MPI_Bcast(b_panel, ctx->block_elems, MPI_DOUBLE, k, ctx->col_comm);
        timing->broadcast += timer_now() - comm_start;

        if (ctx->col == k) {
            timing->bytes_sent += (long long)(ctx->grid_size - 1) * bytes_per_block;
        } else {
            timing->bytes_received += bytes_per_block;
        }
        if (ctx->row == k) {
            timing->bytes_sent += (long long)(ctx->grid_size - 1) * bytes_per_block;
        } else {
            timing->bytes_received += bytes_per_block;
        }

        double compute_start = timer_now();
        matrix_multiply_add(a_panel, b_panel, local_c,
                            ctx->block_size, ctx->block_size, ctx->block_size);
        timing->compute += timer_now() - compute_start;
    }

    free(a_panel);
    free(b_panel);
}

static Matrix gather_result(const GridContext *ctx, const AppConfig *config,
                            double *local_c, PhaseTiming *timing) {
    double start = timer_now();
    Matrix global_c;
    global_c.rows = 0;
    global_c.cols = 0;
    global_c.data = NULL;
    int bytes_per_block = ctx->block_elems * (int)sizeof(double);

    if (ctx->grid_rank == 0) {
        global_c = matrix_create(config->n, config->n);
        matrix_paste_block(&global_c, local_c, ctx->block_size, ctx->block_size, 0, 0);

        for (int r = 1; r < ctx->world_size; r++) {
            int coords[2];
            MPI_Cart_coords(ctx->grid_comm, r, 2, coords);

            double *tmp_c = checked_calloc((size_t)ctx->block_elems, sizeof(double));
            MPI_Recv(tmp_c, ctx->block_elems, MPI_DOUBLE, r, 20, ctx->grid_comm, MPI_STATUS_IGNORE);
            timing->bytes_received += bytes_per_block;
            matrix_paste_block(&global_c, tmp_c, ctx->block_size, ctx->block_size, coords[0], coords[1]);
            free(tmp_c);
        }
    } else {
        MPI_Send(local_c, ctx->block_elems, MPI_DOUBLE, 0, 20, ctx->grid_comm);
        timing->bytes_sent += bytes_per_block;
    }

    timing->gather += timer_now() - start;
    return global_c;
}

static double verify_result(const AppConfig *config, const Matrix *global_c) {
    if (!config->verify || !global_c || !global_c->data) {
        return -1.0;
    }

    Matrix global_a = matrix_create(config->n, config->n);
    Matrix global_b = matrix_create(config->n, config->n);
    Matrix expected = matrix_create(config->n, config->n);

    matrix_init(&global_a, config->init_mode, config->seed, 0);
    matrix_init(&global_b, config->init_mode, config->seed, 1);
    matrix_multiply_serial(&global_a, &global_b, &expected);

    double error = matrix_max_abs_error(global_c, &expected);
    matrix_destroy(&global_a);
    matrix_destroy(&global_b);
    matrix_destroy(&expected);
    return error;
}

int summa_run(const AppConfig *config, RunResult *result, RankMetrics **all_metrics_out,
              int *metrics_count_out, MPI_Comm world) {
    GridContext ctx;
    if (!grid_create(world, config->n, &ctx)) {
        return 0;
    }

    PhaseTiming timing;
    timing_reset(&timing);

    double *local_a = checked_calloc((size_t)ctx.block_elems, sizeof(double));
    double *local_b = checked_calloc((size_t)ctx.block_elems, sizeof(double));
    double *local_c = checked_calloc((size_t)ctx.block_elems, sizeof(double));

    MPI_Barrier(ctx.grid_comm);
    double total_start = timer_now();

    distribute_blocks(config, &ctx, local_a, local_b, &timing);
    run_summa_loop(&ctx, local_a, local_b, local_c, &timing);
    Matrix global_c = gather_result(&ctx, config, local_c, &timing);

    MPI_Barrier(ctx.grid_comm);
    timing.total = timer_now() - total_start;
    timing.communication = timing.distribution + timing.broadcast + timing.gather;
    timing.idle = timing.total - timing.compute - timing.communication;
    if (timing.idle < 0.0) {
        timing.idle = 0.0;
    }

    RankMetrics local_metrics;
    metrics_fill(&local_metrics, ctx.world_rank, ctx.grid_rank, ctx.row, ctx.col,
                 ctx.block_size, ctx.block_size, &timing);

    RankMetrics *all_metrics = NULL;
    if (ctx.grid_rank == 0) {
        all_metrics = checked_calloc((size_t)ctx.world_size, sizeof(RankMetrics));
    }

    MetricsSummary summary;
    metrics_reduce_summary(&local_metrics, &summary, ctx.grid_comm);
    metrics_gather_all(&local_metrics, all_metrics, 0, ctx.grid_comm);

    if (ctx.grid_rank == 0) {
        result->n = config->n;
        result->process_count = ctx.world_size;
        result->grid_size = ctx.grid_size;
        result->block_size = ctx.block_size;
        result->verification_error = verify_result(config, &global_c);
        result->checksum = matrix_checksum(&global_c);
        result->summary = summary;

        *all_metrics_out = all_metrics;
        *metrics_count_out = ctx.world_size;
        matrix_destroy(&global_c);
    }

    free(local_a);
    free(local_b);
    free(local_c);
    grid_destroy(&ctx);
    return 1;
}
