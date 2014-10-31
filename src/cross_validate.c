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


struct validator
{
    double **param_vecs;
    rxmap *classes;
    rxmap *tokens;
    int *totalptr;
    int *rightptr;
};

void validator_per_row(MYSQL_ROW row, void *arg)
{
    struct validator v = * (struct validator *) arg;
    tok_all_to_lowerc(row[1]);
    int actual = rxmap_get(v.classes, row[1]);
    if(actual != -1)
    {
        *(v.totalptr) += 1;
        int size;
        char **toks = tok_words(row[0], &size);
        int *indeces = tokens_to_indeces_filtered(v.tokens, toks, size);
        int guess = top_score_index(size, indeces, v.classes->size, v.param_vecs);
        if(guess == actual)
            *(v.rightptr) += 1;
        free(toks);
        free(indeces);
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

    int total, right;
    total = right = 0;

    struct validator v =
    {
        .param_vecs = param_vecs,
        .totalptr = &total,
        .rightptr = &right,
        .classes = res.classes,
        .tokens = res.tokens
    };

    struct mysql_visitor validate_vis =
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 1,
        .row_count = atoi(argv[4]),
        .per_row = &validator_per_row,
        .arg = &v
    };

    MYSQL *connc = mysql_start();
    mysql_visit_rows(connc, validate_vis);
    printf("%f\n", right/(double)total);
    mysql_finish(connc);
    free_param_vecs(res.classes->size, param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}


