#include "timing.h"

#include <float.h>
#include <stddef.h>

double timer_now(void) {
    return MPI_Wtime();
}

void timing_reset(PhaseTiming *timing) {
    timing->distribution = 0.0;
    timing->broadcast = 0.0;
    timing->compute = 0.0;
    timing->gather = 0.0;
    timing->total = 0.0;
    timing->communication = 0.0;
    timing->idle = 0.0;
    timing->bytes_sent = 0;
    timing->bytes_received = 0;
}

void metrics_fill(RankMetrics *metrics, int world_rank, int grid_rank,
                  int row, int col, int local_rows, int local_cols,
                  const PhaseTiming *timing) {
    metrics->world_rank = world_rank;
    metrics->grid_rank = grid_rank;
    metrics->grid_row = row;
    metrics->grid_col = col;
    metrics->local_rows = local_rows;
    metrics->local_cols = local_cols;
    metrics->timing = *timing;
}

static void reduce_double(double value, double *min_value, double *max_value,
                          double *sum_value, MPI_Comm comm) {
    MPI_Reduce(&value, min_value, 1, MPI_DOUBLE, MPI_MIN, 0, comm);
    MPI_Reduce(&value, max_value, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
    MPI_Reduce(&value, sum_value, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
}

void metrics_reduce_summary(const RankMetrics *local, MetricsSummary *summary, MPI_Comm comm) {
    int size = 1;
    MPI_Comm_size(comm, &size);

    double total_sum = 0.0;
    double compute_sum = 0.0;
    double comm_sum = 0.0;
    double idle_min = 0.0;
    double idle_sum = 0.0;

    reduce_double(local->timing.total, &summary->min_total, &summary->max_total, &total_sum, comm);
    reduce_double(local->timing.compute, &summary->min_compute, &summary->max_compute, &compute_sum, comm);
    reduce_double(local->timing.communication, &summary->min_comm, &summary->max_comm, &comm_sum, comm);
    MPI_Reduce(&local->timing.idle, &idle_min, 1, MPI_DOUBLE, MPI_MIN, 0, comm);
    MPI_Reduce(&local->timing.idle, &summary->max_idle, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
    MPI_Reduce(&local->timing.idle, &idle_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
    MPI_Reduce(&local->timing.bytes_sent, &summary->total_bytes_sent, 1, MPI_LONG_LONG, MPI_SUM, 0, comm);
    MPI_Reduce(&local->timing.bytes_received, &summary->total_bytes_received, 1, MPI_LONG_LONG, MPI_SUM, 0, comm);

    summary->avg_total = total_sum / (double)size;
    summary->avg_compute = compute_sum / (double)size;
    summary->avg_comm = comm_sum / (double)size;
    summary->load_imbalance_percent = 0.0;

    if (summary->max_total > 0.0) {
        double idle_gap = summary->max_idle - idle_min;
        summary->load_imbalance_percent = idle_gap * 100.0 / summary->max_total;
    }
}

void metrics_gather_all(const RankMetrics *local, RankMetrics *all, int root, MPI_Comm comm) {
    MPI_Gather(local, (int)sizeof(RankMetrics), MPI_BYTE,
               all, (int)sizeof(RankMetrics), MPI_BYTE,
               root, comm);
}
