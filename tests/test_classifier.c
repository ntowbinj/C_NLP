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

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        fprintf(stderr, "args: resources file, classifier type, input string\n\n");
        fprintf(stderr, "classifier types: MNB, CNB");
        exit(1);
    }

    int reps = 1;

    if(argc == 5)
        reps = atoi(argv[4]);

    struct raw_resources res = read_raw_resources(argv[1]);

    float **param_vecs;
    if(!strcmp(argv[2], "CNB"))
    {
        param_vecs = CNB_get_param_vecs(
                res.classes->size,
                res.tokens->size,
                res.class_counts,
                res.occurrences_matrix,
                0.4);
    }
    else
    {
        param_vecs = get_param_vecs(
                res.classes->size,
                res.tokens->size,
                res.class_counts,
                res.occurrences_matrix,
                &default_estimator,
                0.4);
    }
    free_raw_resources_arrays(res);

    for(int i = 0; i<reps; i++)
    {
        int size;
        char *string_lit = argv[3];
        char string[strlen(string_lit) + 1];
        strcpy(string, string_lit);
        char **toks = tok_words(string, &size);
        int *indeces = tokens_to_indeces_filtered(res.tokens, toks, size);
        int class_index = top_score_index(size, indeces, res.classes->size, param_vecs);

        free(toks);
        free(indeces);

        char *class = res.classes->keys->arr[class_index];
        if(reps == 1) printf("CHOICE: %s\n", class);
    }


    free_param_vecs(res.classes->size, param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);


}

