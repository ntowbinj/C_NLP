#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters.h"

#define MAXLEN 50

//#include "config.h"

MYSQL *conn;


static inline void per_row_multinom_flat(MYSQL_ROW row, void *arguments)
{
    struct multinom_arg_flat arg = * (struct multinom_arg_flat *) arguments;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(arg.words, toks, size);
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1)
        {
            arg.occurrences[indeces[i]]++;
            *(arg.sum_total_ptr) += 1;
        }
    }
    free(toks);
    free(indeces);
}

static inline void per_row_bernoulli_flat(MYSQL_ROW row, void *arguments)
{
    struct bernoulli_arg_flat arg = * (struct bernoulli_arg_flat *) arguments;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(arg.words, toks, size);
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1 && !arg.zero_slate[indeces[i]])
            arg.zero_slate[indeces[i]] = 1;
    }

    // loop through again, and wherever zero_slate
    // is non-zero, incrememt present_totals once, then
    // set zero_slate to be 0 there so that it's
    // not row_counted again, there by row_counting boolean 
    // zero_slate, not multinomuency, and 'cleaning'
    // zero_slate[]
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1)
        {
            if(arg.zero_slate[indeces[i]])
            {
                arg.present_totals[indeces[i]]++;
                *(arg.sum_total_ptr) += 1;
                arg.zero_slate[indeces[i]] = 0;
            }
        }
    }
    free(toks);
    free(indeces);
}

static inline void per_row_multinom_matrix(MYSQL_ROW row, void *arguments)
{
    struct multinom_arg_matrix arg = * (struct multinom_arg_matrix *) arguments;
    tok_all_to_lowerc(row[1]);
    int subdex = rxmap_get(arg.subs, row[1]);
    if(subdex != -1)
    {
        arg.sub_counts[subdex]++;
        struct multinom_arg_flat flat_arg = 
        {
            .occurrences = arg.occurrences_matrix[subdex],
            .sum_total_ptr = arg.sum_totals + subdex,
            .words = arg.words
        };
        per_row_multinom_flat(row, &flat_arg);
    }
}

static inline void per_row_bernoulli_matrix(MYSQL_ROW row, void *arguments)
{
    struct bernoulli_arg_matrix arg = * (struct bernoulli_arg_matrix *) arguments;
    tok_all_to_lowerc(row[1]);
    int subdex = rxmap_get(arg.subs, row[1]);
    if(subdex != -1)
    {
        arg.sub_counts[subdex]++;
        struct bernoulli_arg_flat flat_arg = 
        {
            .zero_slate = arg.zero_slate,
            .present_totals = arg.present_totals_matrix[subdex],
            .sum_total_ptr = arg.sum_totals + subdex,
            .words = arg.words
        };
        per_row_bernoulli_flat(row, &flat_arg);
    }
}



// occurrences should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// occurrences[i] will have the total number of times
// word i appeared.
void multinom_flat(double *sum_total_ptr, int *occurrences, rxmap *words, int row_count)
{
    struct multinom_arg_flat arg = {.occurrences = occurrences, .sum_total_ptr = sum_total_ptr, .words = words};
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_multinom_flat, &arg);
}

// present_total should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// present_total[i] will have the number of rows
// in which word i was present
void bernoulli_flat(double *sum_total_ptr, int *present_totals, rxmap *words, int row_count)
{
    int zero_slate[words->size];
    memset(zero_slate, 0, sizeof(*zero_slate)*words->size);
    struct bernoulli_arg_flat arg = 
    {
        .zero_slate = zero_slate,
        .present_totals = present_totals,
        .sum_total_ptr = sum_total_ptr,
        .words = words
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_bernoulli_flat, &arg);
}

void bernoulli_matrix(int *sub_counts, double *sum_totals, int **present_totals_matrix, rxmap *subs, rxmap *words, int row_count)
{
    int zero_slate[words->size];
    memset(zero_slate, 0, sizeof(*zero_slate)*words->size);
    struct bernoulli_arg_matrix arg = 
    {
        .zero_slate = zero_slate,
        .sub_counts = sub_counts,
        .present_totals_matrix = present_totals_matrix,
        .sum_totals = sum_totals,
        .words = words,
        .subs = subs
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT_AND_SUB, row_count, &per_row_bernoulli_matrix, &arg);
}

void multinom_matrix(int *sub_counts, double *sum_totals, int **occurrences_matrix, rxmap *subs, rxmap *words, int row_count)
{
    struct multinom_arg_matrix arg = 
    {
        .occurrences_matrix = occurrences_matrix, 
        .sum_totals = sum_totals,
        .sub_counts = sub_counts,
        .words = words,
        .subs = subs
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT_AND_SUB, row_count, &per_row_multinom_matrix, &arg);
}

void init_raw_resources_arrays(struct raw_resources *resptr, int num_classes, int num_tokens)
{
    resptr->class_counts = calloc(num_classes, sizeof(*resptr->class_counts));
    resptr->sum_totals = calloc(num_classes, sizeof(*resptr->sum_totals));
    resptr->occurrences_matrix = malloc(num_classes*sizeof(*resptr->occurrences_matrix));
    for(int i = 0; i<num_classes; i++)
        resptr->occurrences_matrix[i] = calloc(num_tokens, sizeof(**resptr->occurrences_matrix));
}

struct raw_resources build_raw_resources(struct build_params params)
{
    rxmap *classes = build_wordlist(params.classes_filepath, MAXLEN);
    rxmap *tokens = build_wordlist(params.tokens_filepath, MAXLEN);

    struct raw_resources ret;
    init_raw_resources_arrays(&ret, classes->size, tokens->size);
    conn = mysql_start();
    ret.classes = classes;
    ret.tokens = tokens;

    multinom_matrix(
            ret.class_counts,
            ret.sum_totals,
            ret.occurrences_matrix,
            ret.classes,
            ret.tokens,
            params.training_size);

    mysql_finish(conn);
    return ret;
}

struct raw_resources read_raw_resources(char *file_path)
{
    struct raw_resources res;
    FILE *fp = fopen(file_path, "r");
    rxmap *classes, *tokens;
    classes = rxmap_new();
    tokens = rxmap_new();
    int num_classes, num_tokens;

    // number of classes
    fread(&num_classes, sizeof(res.classes->size), 1, fp);

    // number of tokens
    fread(&num_tokens, sizeof(res.tokens->size), 1, fp);

    init_raw_resources_arrays(&res, num_classes, num_tokens);

    // classes
    build_load_n_words(fp, num_classes, classes, MAXLEN);

    // tokens
    build_load_n_words(fp, num_tokens, tokens, MAXLEN);

    // matrix
    for(int i = 0; i<num_classes; i++){
        for(int j = 0; j<num_tokens; j++)
        {
            fread(&res.occurrences_matrix[i][j], sizeof(**res.occurrences_matrix), 1, fp);
        }
    }

    // class_counts
    for(int i = 0; i<num_classes; i++)
    {
        fread(&res.class_counts[i], sizeof(*res.class_counts), 1, fp);
    }

    // sum_totals
    for(int i = 0; i<num_classes; i++)
    {
        fread(&res.sum_totals[i], sizeof(*res.sum_totals), 1, fp);
    }
    fclose(fp);
    //printf("%d\n", classes->size);
    //printf("%d\n", tokens->size);
    res.classes = classes;
    res.tokens = tokens;
    return res;
}

void store_raw_resources(struct raw_resources res, char* file_path)
{
    FILE *fp = fopen(file_path, "w");
    // number of classes
    fwrite(&res.classes->size, sizeof(res.classes->size), 1, fp);
    // number of tokens
    fwrite(&res.tokens->size, sizeof(res.tokens->size), 1, fp);

    // classes
    for(int i = 0; i<res.classes->size; i++)
    {
        fprintf(fp, "%s\n", (char *) res.classes->keys->arr[i]);
    }

    // tokens
    for(int i = 0; i<res.tokens->size; i++)
    {
        fprintf(fp, "%s\n", (char *) res.tokens->keys->arr[i]);
    }

    // matrix
    for(int i = 0; i<res.classes->size; i++)
    {
        for(int j = 0; j<res.tokens->size; j++)
        {
            fwrite(&res.occurrences_matrix[i][j], sizeof(**res.occurrences_matrix), 1, fp);
        }
    }

    // class_counts
    for(int i = 0; i<res.classes->size; i++)
    {
        fwrite(&res.class_counts[i], sizeof(*res.class_counts), 1, fp);
    }

    // sum_totals
    for(int i = 0; i<res.classes->size; i++)
    {
        fwrite(&res.sum_totals[i], sizeof(*res.sum_totals), 1, fp);
    }
    fclose(fp);
}

void free_raw_resources(struct raw_resources resources)
{
    for(int i = 0; i<resources.classes->size; i++)
    {
        free(resources.occurrences_matrix[i]);
    }
    free(resources.occurrences_matrix);
    free(resources.class_counts);
    free(resources.sum_totals);
    rxmap_delete(resources.classes);
    rxmap_delete(resources.tokens);
}



