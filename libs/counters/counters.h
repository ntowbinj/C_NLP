#ifndef COUNTERS_H
#define COUNTERS_H
#include "radix_map/rxmap.h"
#include "mysql_help/mysql_help.h"

struct build_params
{
    char *classes_filepath;
    char *tokens_filepath;
    long training_size;
};

struct raw_resources
{
    rxmap *classes;
    rxmap *tokens;
    int **occurrences_matrix;
    int *class_counts;
    double *sum_totals;
};


struct bernoulli_arg_flat
{
    int *zero_slate;
    int *present_totals;
    double *sum_total_ptr;
    rxmap *tokens;
};

struct multinom_arg_flat
{
    int *occurrences;
    double *sum_total_ptr;
    rxmap *tokens;
};

struct bernoulli_arg_matrix
{
    int *zero_slate;
    int **present_totals_matrix;
    int *class_counts;
    double *sum_totals;
    rxmap *classes;
    rxmap *tokens;
};

struct multinom_arg_matrix
{
    int **occurrences_matrix;
    int *class_counts;
    double *sum_totals;
    rxmap *classes;
    rxmap *tokens;
};

struct raw_resources build_raw_resources(struct build_params params);

void bernoulli_matrix(int *class_counts, double *sum_totals, int **present_totals_matrix, rxmap *classes, rxmap *tokens, int row_count);

void multinom_matrix(int *class_counts, double *sum_totals, int **occurrences_matrix, rxmap *classes, rxmap *tokens, int row_count);


void free_raw_resources_arrays(struct raw_resources resources);

void store_raw_resources(struct raw_resources res, char* file_path);

struct raw_resources read_raw_resources(char *file_path);

#endif
