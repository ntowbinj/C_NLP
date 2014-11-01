#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "radix_map/rxmap.h"

typedef double *(*param_estimator)(int, int, int*, float); 

/**
 * the most basic, unsophisticated estimator that will be used
 * if the param_estimator arg to get_param_vecs is NULL
 */
double *default_estimator(int num_tokens, int class_samplesize, int *occurrences, float smoothing);

/**
 * get the class index of the classification
 * from the paramater vectors
 */
int top_score_index(int num_tokens, int *indeces, int class_samplesize, double **params);

/**
 * get the top k guesses in order
 * assumes k <= num_classes
 * inefficiently uses full qsort even for small k
 */
int *top_k_score_indeces(
        int num_tokens,
        int *indeces,
        int num_classes,
        double **param_vecs,
        int k);

/**
 * given dimensions, a param_estimator, and a smoothing arg,
 * and non-normalized count vectors,
 * create a list of the parameter vectors in log space
 */
double **get_param_vecs(
        int num_classes,
        int num_tokens,
        int *class_samplesizes,
        int **vectors,
        param_estimator est,
        float smootharg);

/**
 * conveniently frees the parameter list, non-essential
 */
void free_param_vecs(int num_classes, double **vectors);

#endif
