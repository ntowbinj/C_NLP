#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"

int main(int argc, char *argv[])
{
    struct raw_resources res = read_raw_resources("/home/n/programming/cstuff/analysis/data/try2.bays");


    double *log_param_sets[res.classes->size];
    for(int i = 0; i<res.classes->size; i++)
        log_param_sets[i] = log_param_estimations(res.tokens->size, res.class_counts[i], res.occurrences_matrix[i], 0.1);
    free_raw_resources_arrays(res);

    int size;
    char *string_lit = "what's the name of that in new mexico, albequerqe, meth, drugs, dealer, chemistry";
    char string[strlen(string_lit) + 1];
    strcpy(string, string_lit);
    char **toks = tok_words(string, &size);
    int *indeces = tokens_to_indeces_filtered(res.tokens, toks, size);
    int class_index = top_score_index(size, indeces, res.classes->size, log_param_sets);

    free(toks);
    free(indeces);

    char *class = res.classes->keys->arr[class_index];
    printf("CHOICE: %s\n", class);


    for(int i = 0; i<res.classes->size; i++)
    {
        free(log_param_sets[i]);
    }
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
    //
    //char hold[100];
    //fgets(hold, 10, stdin);


}


