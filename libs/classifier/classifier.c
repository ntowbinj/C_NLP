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


static float score_for_class(int num_tokens, int *indeces, float *params)
{
    float score = 0;
    for(int i = 0; i<num_tokens; i++)
    {
        if(indeces[i] != -1)
            score += params[indeces[i]];
    }
    return score;
}

int *top_k_score_indeces(
        int num_tokens,
        int *indeces,
        int num_classes,
        float **param_vecs,
        int k)
{
    float scores[num_classes];
    for(int i = 0; i<num_classes; i++)
        scores[i] = score_for_class(num_tokens, indeces, param_vecs[i]);
    int *index_remap = util_sortby_remap_flt(scores, num_classes);
    int *ret = malloc(k*sizeof(*ret));
    for(int i = 0; i<k; i++)
        ret[i] = index_remap[num_classes - i - 1];
    free(index_remap);
    return ret;
}


int top_score_index(int num_tokens, int *indeces, int num_classes, float **param_vecs)
{
    int maxdex = -1;
    float max_score = -DBL_MAX;
    float score;
    for(int i = 0; i<num_classes; i++)
    {
        score = score_for_class(num_tokens, indeces, param_vecs[i]);
        if(score > max_score && score)
        {
            max_score = score;
            maxdex = i;
        }
    }
    return maxdex;
}

void free_param_vecs(int num_classes, float **vectors)
{
    for(int i = 0; i<num_classes; i++)
        free(vectors[i]);
    free(vectors);
}


float **get_param_vecs(
        int num_classes,
        int num_tokens,
        int *class_samplesizes,
        int **vectors,
        param_estimator est,
        float smootharg)
{
    if(!est) est = &(default_estimator);
    float **sets = malloc(num_classes*sizeof(*sets));
    for(int i = 0; i<num_classes; i++)
    {
        sets[i] = (*est)(num_tokens, class_samplesizes[i], vectors[i], smootharg);
    }
    return sets;
}


float *default_estimator(int num_tokens, int class_samplesize, int *occurrences, float smoothing)
{
    float *estimations = malloc(num_tokens*sizeof(*estimations));
    long double total = 0;
    for(int i = 0; i<num_tokens; i++)
    {
        total += occurrences[i];
    }
    double log_denom = log2l(total + smoothing*num_tokens);
    for(int i = 0; i<num_tokens; i++)
    {
        float log_numer = log2(occurrences[i] + smoothing);
        estimations[i] = log_numer - log_denom;
    }
    return estimations;
}


/*float std_dev(int num_classes, int token_index, int **matrix)
{

    long total = 0;
    for(int i = 0; i<num_classes; i++)
        total += matrix[i][token_index];
    float mean = ((float) total)/num_classes;
    float variance = 0;
    for(int i = 0; i<num_classes; i++)
        variance += pow((matrix[i][token_index] - mean), 2);
    return pow(sqrt(variance), .1);
}*/

