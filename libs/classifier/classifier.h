#ifndef CLASSIFIER_H
#define CLASSIFIER_H

double *log_param_estimations(int len, int class_count, int *occurrences, float smoothing);
double score_for_class(int len, int *indeces, double *log_params);
int top_score_index(int len, int *indeces, int class_count, double **log_params);

#endif