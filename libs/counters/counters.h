#ifndef COUNTERS_H
#define COUNTERS_H
#include "radix_map/rxmap.h"
#include "mysql_help/mysql_help.h"

typedef enum {BERNOULLI, MULTINOM} model_t;

struct build_params
{
    char *classes_filepath;
    char *tokens_filepath;
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

struct raw_resources build_raw_resources(struct build_params params, struct mysql_visitor visitor, model_t model);

void bernoulli_matrix(
        int *class_counts,
        double *sum_totals,
        int **present_totals_matrix,
        rxmap *classes,
        rxmap *tokens,
        struct mysql_visitor visitor);

void multinom_matrix(
        int *class_counts,
        double *sum_totals,
        int **occurrences_matrix,
        rxmap *classes,
        rxmap *tokens,
        struct mysql_visitor visitor);


void free_raw_resources_arrays(struct raw_resources resources);

void store_raw_resources(struct raw_resources res, char* file_path);

struct raw_resources read_raw_resources(char *file_path);

#endif
