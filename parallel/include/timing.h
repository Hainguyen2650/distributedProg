#ifndef TIMING_H
#define TIMING_H

#include <mpi.h>

typedef struct {
    double distribution;
    double broadcast;
    double compute;
    double gather;
    double total;
    double communication;
    double idle;
    long long bytes_sent;
    long long bytes_received;
} PhaseTiming;

typedef struct {
    int world_rank;
    int grid_rank;
    int grid_row;
    int grid_col;
    int local_rows;
    int local_cols;
    PhaseTiming timing;
} RankMetrics;

typedef struct {
    double min_total;
    double max_total;
    double avg_total;
    double min_compute;
    double max_compute;
    double avg_compute;
    double min_comm;
    double max_comm;
    double avg_comm;
    double max_idle;
    double load_imbalance_percent;
    long long total_bytes_sent;
    long long total_bytes_received;
} MetricsSummary;

double timer_now(void);
void timing_reset(PhaseTiming *timing);
void metrics_fill(RankMetrics *metrics, int world_rank, int grid_rank,
                  int row, int col, int local_rows, int local_cols,
                  const PhaseTiming *timing);
void metrics_reduce_summary(const RankMetrics *local, MetricsSummary *summary, MPI_Comm comm);
void metrics_gather_all(const RankMetrics *local, RankMetrics *all, int root, MPI_Comm comm);

#endif
