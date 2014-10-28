#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"


double *log_param_estimations(int len, int class_count, int *occurrences, float smoothing)
{
    double *estimations = malloc(len*sizeof(*estimations));
    double total = 0;
    for(int i = 0; i<len; i++)
    {
        total += occurrences[i];
    }
    double log_denom = log2(total + smoothing*len);
    for(int i = 0; i<len; i++)
    {
        double log_numer = log2(occurrences[i] + smoothing);
        estimations[i] = log_numer - log_denom;
    }
    return estimations;
}

double score_for_class(int len, int *indeces, double *log_params)
{
    double score = 0;
    for(int i = 0; i<len; i++)
    {
        if(indeces[i] != -1)
            score += log_params[indeces[i]];
    }
    return score;
}

int top_score_index(int len, int *indeces, int class_count, double **log_params)
{
    int maxdex = 0;
    double max_score = -DBL_MAX;
    double score;
    for(int i = 0; i<class_count; i++)
    {
        score = score_for_class(len, indeces, log_params[i]);
        if(score > max_score)
        {
            max_score = score;
            maxdex = i;
        }
    }
    return maxdex;
}

/*struct classifier classifier_from_raw_resources(struct raw_resources res)
{*/


/*
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "wrong args.\ngive number.\n");
        exit(1);
    }
    struct build_params params = 
    {
        .classes_filepath = PROJECT_ROOT""SUBLIST,
        .tokens_filepath = PROJECT_ROOT""WORDLIST,
        .training_size = atoi(argv[1])
    };

    //struct raw_resources res_built = build_raw_resources(params);
    //store_raw_resources(res_built, "/home/n/programming/cstuff/analysis/data/try.bays");
    struct raw_resources res = read_raw_resources("/home/n/programming/cstuff/analysis/data/try.bays");


    double *log_param_sets[res.classes->size];
    for(int i = 0; i<res.classes->size; i++)
        log_param_sets[i] = log_param_estimations(res.tokens->size, res.class_counts[i], res.occurrences_matrix[i], 0.1);

    int size;
    //char *string_lit = "politics and government, voters and the public eye";
    char *string_lit = "man, I love that really cool called the walking dead zombies. best group.";
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
    //free_raw_resources(res_built);
    free_raw_resources(res);


}
*/
