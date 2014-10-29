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
    long double total = 0;
    for(int i = 0; i<len; i++)
    {
        total += occurrences[i];
    }
    double log_denom = log2l(total + smoothing*len);
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

