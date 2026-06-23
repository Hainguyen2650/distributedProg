#ifndef ARGS_H
#define ARGS_H

#define OUTPUT_PATH_MAX 256

typedef enum {
    INIT_PATTERN = 0,
    INIT_IDENTITY = 1,
    INIT_RANDOM = 2
} InitMode;

typedef struct {
    int n;
    int verify;
    int print_matrix;
    int csv;
    int per_rank_csv;
    int seed;
    InitMode init_mode;
    char output_prefix[OUTPUT_PATH_MAX];
} AppConfig;

void config_set_defaults(AppConfig *config);
int parse_args(int argc, char **argv, AppConfig *config, char *error, int error_size);
void print_usage(const char *program);
const char *init_mode_name(InitMode mode);

#endif
