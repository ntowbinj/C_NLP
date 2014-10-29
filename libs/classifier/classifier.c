#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "classifier/classifier.h"


static double score_for_class(int num_tokens, int *indeces, double *params)
{
    double score = 0;
    for(int i = 0; i<num_tokens; i++)
    {
        if(indeces[i] != -1)
            score += params[indeces[i]];
    }
    return score;
}

int top_score_index(int num_tokens, int *indeces, int class_samplesize, double **param_vecs)
{
    int maxdex = 0;
    double max_score = -DBL_MAX;
    double score;
    for(int i = 0; i<class_samplesize; i++)
    {
        score = score_for_class(num_tokens, indeces, param_vecs[i]);
        if(score > max_score)
        {
            max_score = score;
            maxdex = i;
        }
    }
    return maxdex;
}

void free_param_vecs(int num_classes, double **vectors)
{
    for(int i = 0; i<num_classes; i++)
        free(vectors[i]);
    free(vectors);
}


double **get_param_vecs(
        int num_classes,
        int num_tokens,
        int *class_samplesizes,
        int **vectors,
        param_estimator est,
        float smootharg)
{
    if(!est) est = &(default_estimator);
    double **sets = malloc(num_classes*sizeof(*sets));
    for(int i = 0; i<num_classes; i++)
    {
        sets[i] = (*est)(num_tokens, class_samplesizes[i], vectors[i], smootharg);
    }
    return sets;
}


double *default_estimator(int num_tokens, int class_samplesize, int *occurrences, float smoothing)
{
    double *estimations = malloc(num_tokens*sizeof(*estimations));
    long double total = 0;
    for(int i = 0; i<num_tokens; i++)
    {
        total += occurrences[i];
    }
    double log_denom = log2l(total + smoothing*num_tokens);
    for(int i = 0; i<num_tokens; i++)
    {
        double log_numer = log2(occurrences[i] + smoothing);
        estimations[i] = log_numer - log_denom;
    }
    return estimations;
}


