#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_flag(const char *arg, const char *name) {
    return strcmp(arg, name) == 0;
}

static int parse_int_value(const char *text, int *value) {
    char *end = NULL;
    long parsed = strtol(text, &end, 10);
    if (!text || *text == '\0' || !end || *end != '\0') {
        return 0;
    }
    if (parsed < 0 || parsed > 2147483647L) {
        return 0;
    }
    *value = (int)parsed;
    return 1;
}

static int parse_init_mode(const char *text, InitMode *mode) {
    if (strcmp(text, "pattern") == 0) {
        *mode = INIT_PATTERN;
        return 1;
    }
    if (strcmp(text, "identity") == 0) {
        *mode = INIT_IDENTITY;
        return 1;
    }
    if (strcmp(text, "random") == 0) {
        *mode = INIT_RANDOM;
        return 1;
    }
    return 0;
}

static void set_error(char *error, int error_size, const char *message) {
    if (error && error_size > 0) {
        snprintf(error, (size_t)error_size, "%s", message);
    }
}

void config_set_defaults(AppConfig *config) {
    config->n = 8;
    config->verify = 1;
    config->print_matrix = 0;
    config->csv = 0;
    config->per_rank_csv = 0;
    config->seed = 1;
    config->init_mode = INIT_PATTERN;
    snprintf(config->output_prefix, sizeof(config->output_prefix), "%s", "summa_result");
}

int parse_args(int argc, char **argv, AppConfig *config, char *error, int error_size) {
    config_set_defaults(config);

    for (int i = 1; i < argc; i++) {
        if (is_flag(argv[i], "--help") || is_flag(argv[i], "-h")) {
            return 2;
        }

        if (is_flag(argv[i], "--n") || is_flag(argv[i], "-n")) {
            if (i + 1 >= argc || !parse_int_value(argv[i + 1], &config->n)) {
                set_error(error, error_size, "Invalid value for --n");
                return 0;
            }
            i++;
            continue;
        }

        if (is_flag(argv[i], "--seed")) {
            if (i + 1 >= argc || !parse_int_value(argv[i + 1], &config->seed)) {
                set_error(error, error_size, "Invalid value for --seed");
                return 0;
            }
            i++;
            continue;
        }

        if (is_flag(argv[i], "--init")) {
            if (i + 1 >= argc || !parse_init_mode(argv[i + 1], &config->init_mode)) {
                set_error(error, error_size, "Invalid value for --init");
                return 0;
            }
            i++;
            continue;
        }

        if (is_flag(argv[i], "--output-prefix")) {
            if (i + 1 >= argc) {
                set_error(error, error_size, "Missing value for --output-prefix");
                return 0;
            }
            snprintf(config->output_prefix, sizeof(config->output_prefix), "%s", argv[i + 1]);
            i++;
            continue;
        }

        if (is_flag(argv[i], "--no-verify")) {
            config->verify = 0;
            continue;
        }

        if (is_flag(argv[i], "--print-matrix")) {
            config->print_matrix = 1;
            continue;
        }

        if (is_flag(argv[i], "--csv")) {
            config->csv = 1;
            continue;
        }

        if (is_flag(argv[i], "--per-rank-csv")) {
            config->per_rank_csv = 1;
            continue;
        }

        if (argv[i][0] != '-') {
            if (!parse_int_value(argv[i], &config->n)) {
                set_error(error, error_size, "Invalid positional matrix size");
                return 0;
            }
            continue;
        }

        set_error(error, error_size, "Unknown command-line option");
        return 0;
    }

    if (config->n <= 0) {
        set_error(error, error_size, "Matrix size must be positive");
        return 0;
    }

    return 1;
}

void print_usage(const char *program) {
    printf("Usage: %s [N] [options]\n", program);
    printf("\n");
    printf("Parallel matrix multiplication using the SUMMA algorithm.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -n, --n <N>              Matrix size. N x N matrices are multiplied.\n");
    printf("  --init <mode>            pattern, identity, or random.\n");
    printf("  --seed <S>               Seed used by random initialization.\n");
    printf("  --no-verify              Skip serial verification on rank 0.\n");
    printf("  --print-matrix           Print result matrix when N is small.\n");
    printf("  --csv                    Print one CSV summary line.\n");
    printf("  --per-rank-csv           Print per-rank timing CSV lines.\n");
    printf("  --output-prefix <name>   Prefix stored in CSV output.\n");
    printf("  -h, --help               Show this message.\n");
    printf("\n");
    printf("Process-count rule:\n");
    printf("  This implementation uses a q x q process grid, so P must be a perfect square.\n");
    printf("  N must be divisible by q. Examples: -np 4 with N=1000, -np 9 with N=999.\n");
}

const char *init_mode_name(InitMode mode) {
    switch (mode) {
    case INIT_PATTERN:
        return "pattern";
    case INIT_IDENTITY:
        return "identity";
    case INIT_RANDOM:
        return "random";
    default:
        return "unknown";
    }
}
