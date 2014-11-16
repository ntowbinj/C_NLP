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

#define MAXLEN 30

struct subset_validator
{
    float **param_vecs;
    rxmap *classes_full;
    rxmap *classes_subset;
    rxmap *tokens;
    int *totalptr;
    int *rightptr;
};

/*void validator_per_row(MYSQL_ROW row, void *arg)
{
    struct subset_validator v = * (struct subset_validator *) arg;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(v.tokens, toks, size);
    tok_all_to_lowerc(row[1]);
    int actual = rxmap_get(v.classes_subset, row[1]);
    if(actual != -1)
    {
        int guess_index = top_score_index(size, indeces, v.classes_full->size, v.param_vecs);
        char *guess_class = v.classes_full->keys->arr[guess_index];
        if(rxmap_get(v.classes_subset, guess_class) != -1) // the guessed class is in the subset
        {
            *(v.totalptr) += 1; // this counts toward the total
            if(!strcmp(guess_class, row[1])) // the guessed class was correct
                *(v.rightptr) += 1; // this counts toward the right guesses
        }
    }
    free(toks);
    free(indeces);
}*/

void validator_per_row(MYSQL_ROW row, void *arg)
{
    struct subset_validator v = * (struct subset_validator *) arg;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(v.tokens, toks, size);
    tok_all_to_lowerc(row[1]);
    int guess_index = top_score_index(size, indeces, v.classes_full->size, v.param_vecs);
    char *guess_class = v.classes_full->keys->arr[guess_index];
    if(rxmap_get(v.classes_subset, guess_class) != -1) // the guess is in the subset
    {
        *(v.totalptr) += 1; // this counts toward the total
        if(!strcmp(guess_class, row[1])) // the guess class was correct
            *(v.rightptr) += 1; // this counts toward the right guesses
    }
    free(toks);
    free(indeces);
}



int main(int argc, char *argv[])
{
    if(argc < 6)
    {
        fprintf(stderr, "args: full classes path, subset class path, tokens path, size of training set, size of validation set\n\n");
        exit(1);
    }

    struct mysql_visitor training_vis = 
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 0,
        .row_count = atoi(argv[4]),
        .per_row = NULL,
        .arg = NULL
    };


    struct build_params params = 
    {
        .classes_filepath = argv[1],
        .tokens_filepath = argv[3]
    };

    struct raw_resources res = build_raw_resources(params, training_vis, MULTINOM); //MULTINOM hardcoded
    float **param_vecs = get_param_vecs(
            res.classes->size,
            res.tokens->size,
            res.class_counts,
            res.occurrences_matrix,
            NULL,
            0.4);
    free_raw_resources_arrays(res);

    int total, right;
    total = right = 0;

    struct subset_validator v =
    {
        .param_vecs = param_vecs,
        .totalptr = &total,
        .rightptr = &right,
        .classes_full = res.classes,
        .classes_subset = build_wordlist(argv[2], MAXLEN),
        .tokens = res.tokens
    };

    struct mysql_visitor validate_vis =
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 1,
        .row_count = atoi(argv[5]),
        .per_row = &validator_per_row,
        .arg = &v
    };

    MYSQL *connc = mysql_start();
    mysql_visit_rows(connc, validate_vis);
    printf("%f\n", right/(float)total);
    mysql_finish(connc);
    free_param_vecs(res.classes->size, param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}


