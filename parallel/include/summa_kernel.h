#ifndef SUMMA_KERNEL_H
#define SUMMA_KERNEL_H

#include "args.h"
#include "output.h"

#include <mpi.h>

int summa_run(const AppConfig *config, RunResult *result, RankMetrics **all_metrics_out,
              int *metrics_count_out, MPI_Comm world);
int process_count_is_square(int process_count, int *grid_size_out);

#endif
