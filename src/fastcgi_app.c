#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"



#define NUM_THREADS 20

int continue_accepting = 1;

double **log_param_sets;
struct raw_resources res;
void free_stuff();
void undersc_to_space(char *content, long len);

void finish_nongracefully(int sig)
{
    if(sig == SIGTERM)
    {
        continue_accepting = 0;
    }
    free_stuff();
    exit(0);
}



static void *func(void *arg)
{
    FCGX_Request request;

    FCGX_InitRequest(&request, 0, 0);

    char **toks;
    int *indeces;
    int free_them = 0;
    while(continue_accepting)
    {
        static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_lock(&accept_mutex);
        int rc = FCGX_Accept_r(&request);
        pthread_mutex_unlock(&accept_mutex);

        if (rc < 0)
            break;


        char *response = "";
        char *content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
        long content_length = atol(content_length_str);
        if(content_length > 0)
        {
            char content[content_length+1];
            content[content_length] = 0;
            FCGX_GetStr(content, content_length, request.in);
            undersc_to_space(content, content_length);
            int size;
            toks = tok_words(content, &size);
            indeces = tokens_to_indeces_filtered(res.tokens, toks, size);
            free_them = 1;
            int class_index = top_score_index(size, indeces, res.classes->size, log_param_sets);
            response = res.classes->keys->arr[class_index];
        }

        FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n%s", response);
        FCGX_Finish_r(&request);

        if(free_them)
        {
            free(toks);
            free(indeces);
            free_them = 0;
        }
    }

    return NULL;
}

void undersc_to_space(char *content, long len)
{
    for(int i = 0; i<len; i++)
    {
        if(content[i] == '_')
        {
            content[i] = ' ';
        }
    }
}


void classifier_init()
{
    res = read_raw_resources("/home/n/programming/cstuff/bayes-server/data/try200subs.bays");
    log_param_sets = malloc(res.classes->size*sizeof(*log_param_sets));
    for(int i = 0; i<res.classes->size; i++)
        log_param_sets[i] = log_param_estimations(res.tokens->size, res.class_counts[i], res.occurrences_matrix[i], 0.1);
    free_raw_resources_arrays(res);
}

void free_stuff()
{
    for(int i = 0; i<res.classes->size; i++)
    {
        free(log_param_sets[i]);
    }
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}

int main(void)
{
    classifier_init();
    pthread_t id[NUM_THREADS];
    signal(SIGTERM, finish_nongracefully);

    FCGX_Init();

    for (int i = 1; i<NUM_THREADS; i++)
        pthread_create(&id[i], NULL, func, NULL);

    func(0); // become peer

    return 0;
}

