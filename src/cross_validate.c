#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"
#include "classifier/cnb_estimator.h"
#include "mysql_help/mysql_help.h"

#define MAX_DOC_LEN 2048
#define BUCKET_SIZE 50

struct validator
{
    double **param_vecs;
    rxmap *classes;
    rxmap *tokens;
    int *totals;
    int *rights;
    int k; 
};

// needs separate function since is less efficient for k = 1
void per_row_top_k(MYSQL_ROW row, void *arg)
{
    struct validator v = * (struct validator *) arg;
    tok_all_to_lowerc(row[1]);
    int actual = rxmap_get(v.classes, row[1]);
    if(actual != -1)
    {
        int doc_len = strlen(row[0]);
        if(doc_len > MAX_DOC_LEN - 1)
            doc_len = MAX_DOC_LEN - 1;
        v.totals[doc_len]++;
        int size;
        char **toks = tok_words(row[0], &size);
        int *indeces = tokens_to_indeces_filtered(v.tokens, toks, size);
        int *guesses = top_k_score_indeces(size, indeces, v.classes->size, v.param_vecs, v.k);
        for(int i = 0; i<v.k; i++)
        {
            if(guesses[i] == actual)
            {
                v.rights[doc_len]++;
                break;
            }
        }
        free(guesses);
        free(toks);
        free(indeces);
    }
}


void per_row(MYSQL_ROW row, void *arg)
{
    struct validator v = * (struct validator *) arg;
    tok_all_to_lowerc(row[1]);
    int actual = rxmap_get(v.classes, row[1]);
    if(actual != -1)
    {
        int doc_len = strlen(row[0]);
        if(doc_len > MAX_DOC_LEN - 1)
            doc_len = MAX_DOC_LEN - 1;
        v.totals[doc_len]++;
        int size;
        char **toks = tok_words(row[0], &size);
        int *indeces = tokens_to_indeces_filtered(v.tokens, toks, size);
        int guess = top_score_index(size, indeces, v.classes->size, v.param_vecs);
        if(guess == actual)
            v.rights[doc_len]++;
        free(toks);
        free(indeces);
    }
}

double aggregate_avg(int *totals, int *rights, int len)
{
    int total, right;
    total = right = 0;
    for(int i = 0; i<len; i++)
    {
        total += totals[i];
        right += rights[i];
    }
    return ((double) right)/total;
}

void csv_over_doc_len(int *totals, int *rights, int len)
{
    for(int i = 0; i<len; i+=BUCKET_SIZE)
    {
        int bucket_right = 0;
        int bucket_total = 0;
        int j;
        for(j = i; j<i+BUCKET_SIZE && j<len; j++)
        {
            bucket_total +=  totals[j];
            bucket_right += rights[j];
        }
        printf("%d, %f\n", j, ((double) bucket_right)/bucket_total);
    }
}


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "args: classes path, tokens path, size of training set, size of validation set\n\n");
        exit(1);
    }

    struct mysql_visitor training_vis = 
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 0,
        .row_count = atoi(argv[3]),
        .per_row = NULL,
        .arg = NULL
    };


    struct build_params params = 
    {
        .classes_filepath = argv[1],
        .tokens_filepath = argv[2]
    };

    struct raw_resources res = build_raw_resources(params, training_vis, MULTINOM); //MULTINOM hardcoded
    double **param_vecs = get_param_vecs(
            res.classes->size,
            res.tokens->size,
            res.class_counts,
            res.occurrences_matrix,
            NULL,
            0.4);
    free_raw_resources_arrays(res);

    int totals[MAX_DOC_LEN], rights[MAX_DOC_LEN];
    memset(totals, 0, MAX_DOC_LEN*sizeof(*totals));
    memset(rights, 0, MAX_DOC_LEN*sizeof(*rights));

    struct validator v =
    {
        .param_vecs = param_vecs,
        .totals = totals,
        .rights = rights,
        .classes = res.classes,
        .tokens = res.tokens,
        .k = -1
    };

    struct mysql_visitor validate_vis =
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 1,
        .row_count = atoi(argv[4]),
        .per_row = &per_row,
        .arg = &v
    };

    MYSQL *connc = mysql_start();
    mysql_visit_rows(connc, validate_vis);
    //printf("%f\n", aggregate_avg(totals, rights, MAX_DOC_LEN));
    csv_over_doc_len(totals, rights, MAX_DOC_LEN);
    mysql_finish(connc);
    free_param_vecs(res.classes->size, param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}


