#include "args.h"
#include "output.h"
#include "summa_kernel.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

static void print_project_context(int rank) {
    if (rank != 0) {
        return;
    }

    printf("Project context:\n");
    printf("  Algorithm: SUMMA parallel matrix multiplication.\n");
    printf("  Parallelism level: data parallelism.\n");
    printf("  Decomposition: two-dimensional block data decomposition.\n");
    printf("  Mapping: q x q process grid; each rank owns an N/q x N/q block.\n");
    printf("  Communication: blocking MPI point-to-point for scatter/gather and MPI_Bcast for SUMMA panels.\n");
    printf("  Topology: logical 2D Cartesian grid with row and column communicators.\n");
    printf("  Load balance: equal-size matrix blocks when N is divisible by q.\n");
    printf("\n");
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    AppConfig config;
    char error[256];
    int parse_status = parse_args(argc, argv, &config, error, sizeof(error));

    if (parse_status == 2) {
        if (rank == 0) {
            print_usage(argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    if (!parse_status) {
        if (rank == 0) {
            fprintf(stderr, "Argument error: %s\n", error);
            print_usage(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    print_project_context(rank);

    RunResult result;
    RankMetrics *all_metrics = NULL;
    int metrics_count = 0;

    int ok = summa_run(&config, &result, &all_metrics, &metrics_count, MPI_COMM_WORLD);
    if (!ok) {
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        print_human_summary(&config, &result);

        if (config.csv) {
            print_summary_csv_header();
            print_summary_csv(&config, &result);
        }

        if (config.per_rank_csv) {
            print_rank_csv_header();
            print_rank_csv_rows(&config, all_metrics, metrics_count);
        }

        print_work_split_hint();
        free(all_metrics);
    }

    MPI_Finalize();
    return 0;
}
