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

double **CNB_get_param_vecs(
        int num_classes,
        int num_tokens,
        int *class_samplesizes,
        int **vectors,
        float smootharg)
{
    double **vecs = malloc(num_classes*sizeof(*vecs));
    double class_totals[num_classes];
    memset(class_totals, 0, num_classes*sizeof(*class_totals));
    double tok_totals[num_tokens];
    memset(tok_totals, 0, num_tokens*sizeof(*tok_totals));
    long double all_tokens = 0;
    for(int i = 0; i<num_classes; i++)
    {
        for(int j = 0; j<num_tokens; j++)
        {
            int count = vectors[i][j];
            tok_totals[j] += count;
            class_totals[i] += count;
            all_tokens += count;
        }
    }
    printf("%Lf\n", all_tokens);
    for(int i = 0; i<num_classes; i++)
    {
        double log_denom = (double) log2l(all_tokens - class_totals[i] + smootharg*num_tokens);
        vecs[i] = malloc(num_tokens*sizeof(*vecs[i]));
        for(int j = 0; j<num_tokens; j++)
            vecs[i][j] = -1 * (log2(tok_totals[j] - (double) vectors[i][j] + smootharg) - log_denom);
    }
    return vecs;
}



