#ifndef OUTPUT_H
#define OUTPUT_H

#include "args.h"
#include "matrix.h"
#include "timing.h"

typedef struct {
    int n;
    int process_count;
    int grid_size;
    int block_size;
    double verification_error;
    double checksum;
    MetricsSummary summary;
} RunResult;

void print_human_summary(const AppConfig *config, const RunResult *result);
void print_summary_csv_header(void);
void print_summary_csv(const AppConfig *config, const RunResult *result);
void print_rank_csv_header(void);
void print_rank_csv_rows(const AppConfig *config, const RankMetrics *metrics, int count);
void print_work_split_hint(void);

#endif
