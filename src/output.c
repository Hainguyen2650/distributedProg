#include "output.h"

#include <stdio.h>

void print_human_summary(const AppConfig *config, const RunResult *result) {
    printf("SUMMA completed\n");
    printf("  matrix_size: %d x %d\n", result->n, result->n);
    printf("  processes: %d\n", result->process_count);
    printf("  process_grid: %d x %d\n", result->grid_size, result->grid_size);
    printf("  block_size_per_process: %d x %d\n", result->block_size, result->block_size);
    printf("  init_mode: %s\n", init_mode_name(config->init_mode));
    printf("  verification_error: %.6e\n", result->verification_error);
    printf("  checksum: %.6f\n", result->checksum);
    printf("  total_time_with_communication_max: %.6f seconds\n", result->summary.max_total);
    printf("  compute_time_without_communication_max: %.6f seconds\n", result->summary.max_compute);
    printf("  communication_time_max: %.6f seconds\n", result->summary.max_comm);
    printf("  idle_time_max: %.6f seconds\n", result->summary.max_idle);
    printf("  load_imbalance_by_idle_gap: %.2f%%\n", result->summary.load_imbalance_percent);
    printf("  bytes_sent_total: %lld\n", result->summary.total_bytes_sent);
    printf("  bytes_received_total: %lld\n", result->summary.total_bytes_received);
    if (result->summary.load_imbalance_percent > 25.0) {
        printf("  load_balance_status: imbalance_above_25_percent\n");
    } else {
        printf("  load_balance_status: acceptable_under_25_percent\n");
    }
}

void print_summary_csv_header(void) {
    printf("kind,prefix,n,processes,grid,block,init,verify_error,checksum,total_max,compute_max,comm_max,idle_max,imbalance_percent,bytes_sent,bytes_received\n");
}

void print_summary_csv(const AppConfig *config, const RunResult *result) {
    printf("summary,%s,%d,%d,%d,%d,%s,%.12e,%.12f,%.12f,%.12f,%.12f,%.12f,%.6f,%lld,%lld\n",
           config->output_prefix,
           result->n,
           result->process_count,
           result->grid_size,
           result->block_size,
           init_mode_name(config->init_mode),
           result->verification_error,
           result->checksum,
           result->summary.max_total,
           result->summary.max_compute,
           result->summary.max_comm,
           result->summary.max_idle,
           result->summary.load_imbalance_percent,
           result->summary.total_bytes_sent,
           result->summary.total_bytes_received);
}

void print_rank_csv_header(void) {
    printf("kind,prefix,rank,grid_rank,grid_row,grid_col,local_rows,local_cols,total,compute,communication,distribution,broadcast,gather,idle,bytes_sent,bytes_received\n");
}

void print_rank_csv_rows(const AppConfig *config, const RankMetrics *metrics, int count) {
    for (int i = 0; i < count; i++) {
        const RankMetrics *m = &metrics[i];
        printf("rank,%s,%d,%d,%d,%d,%d,%d,%.12f,%.12f,%.12f,%.12f,%.12f,%.12f,%.12f,%lld,%lld\n",
               config->output_prefix,
               m->world_rank,
               m->grid_rank,
               m->grid_row,
               m->grid_col,
               m->local_rows,
               m->local_cols,
               m->timing.total,
               m->timing.compute,
               m->timing.communication,
               m->timing.distribution,
               m->timing.broadcast,
               m->timing.gather,
               m->timing.idle,
               m->timing.bytes_sent,
               m->timing.bytes_received);
    }
}

void print_work_split_hint(void) {
    printf("\nSuggested 4-person implementation split:\n");
    printf("  Part 1: MPI/SUMMA kernel and 2D process-grid mapping.\n");
    printf("  Part 2: Matrix generation, serial baseline, and correctness verification.\n");
    printf("  Part 3: Timing, communication/computation metrics, and load-balance analysis.\n");
    printf("  Part 4: CLI, experiment scripts, CSV outputs, charts, and final report.\n");
}
