#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"

#include "simple_sock_serv/simple_sock_serv.h"

#include "localconfig.h"

float **param_vecs;
struct raw_resources res;
void free_stuff();

void do_it(char *buff, int bufflen, int outfd)
{
    char **toks;
    int *indeces;
    int size;
    toks = tok_words(buff, &size);
    indeces = tokens_to_indeces_filtered(res.tokens, toks, size);
    int class_index = top_score_index(size, indeces, res.classes->size, param_vecs);
    char *response;
    if(class_index == -1)
        response = "";
    else
        response = res.classes->keys->arr[class_index];


    int len = strlen(response);
    write(outfd, response, sizeof(*response)*len);
    free(toks);
    free(indeces);
}

void classifier_init()
{
    res = read_raw_resources(RES_FILE);
    param_vecs = malloc(res.classes->size*sizeof(*param_vecs));
    for(int i = 0; i<res.classes->size; i++)
        param_vecs[i] = default_estimator(res.tokens->size, res.class_counts[i], res.occurrences_matrix[i], 0.4);
    free_raw_resources_arrays(res);
}

void free_stuff()
{
    for(int i = 0; i<res.classes->size; i++)
    {
        free(param_vecs[i]);
    }
    free(param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}

int main(void)
{
    if(fork() != 0)
        return 0;
    setpgrp();
    classifier_init();
    simple_sock_serv(NUM_WORKERS, &do_it, "/var/run/bays2.socket", B_LOG, BUFSZ);
    free_stuff();
}

