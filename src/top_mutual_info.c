#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"

#define MAXLEN 15

struct natural_arg
{
    int *counts;
    rxmap *tokens;
};

float *mutual_info_per_token(
        int num_classes,
        int num_tokens,
        int *class_counts,
        int **matrix)
{
    double class_dist[num_classes];
    double tok_dist[num_tokens];
    double *joint_dists[num_classes];

    double doc_count = 0;
    for(int i = 0; i<num_classes; i++)
    {
        joint_dists[i] = calloc(num_tokens, sizeof(**joint_dists));
        doc_count += class_counts[i];
    }
    for(int i = 0; i<num_classes; i++)
        class_dist[i] = class_counts[i]/doc_count;
    for(int j = 0; j<num_tokens; j++)
    {
        double docs_with_tok = 0;
        for(int i = 0; i<num_classes; i++)
        {
            docs_with_tok += matrix[i][j];
            joint_dists[i][j] += matrix[i][j]/doc_count;
        }
        tok_dist[j] = docs_with_tok/doc_count;
    }
    float *mutual_infos = malloc(num_tokens*sizeof(*mutual_infos));
    for(int j = 0; j<num_tokens; j++)
    {
        double sum = 0;
        for(int i = 0; i<num_classes; i++)
        {
            sum += joint_dists[i][j]*log2(joint_dists[i][j]/(class_dist[i]*tok_dist[j]));
        }
        mutual_infos[j] = sum;
    }
    for(int i = 0; i<num_classes; i++)
        free(joint_dists[i]);
    return mutual_infos;
}

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        fprintf(stderr, "args: classes path, tokens path, size of training set, k for top k\n");
        exit(1);
    }

    struct mysql_visitor training_vis = 
    {
        .query = MYSQL_SELECT_TEXT_AND_CLASS,
        .start_row = 0,
        .which_half = 0,
        .row_count = atoi(argv[3]),
        .per_row = NULL,
        .arg = NULL
    };


    struct build_params params = 
    {
        .classes_filepath = argv[1],
        .tokens_filepath = argv[2]
    };

    struct raw_resources res = build_raw_resources(params, training_vis, BERNOULLI);

    float *mutual_infos = mutual_info_per_token(
            res.classes->size,
            res.tokens->size,
            res.class_counts,
            res.occurrences_matrix);
    free_raw_resources_arrays(res);
    int *index_remap = util_sortby_remap_flt(mutual_infos, res.tokens->size);

    int k = atoi(argv[4]);
    if(k > res.tokens->size)
        k = res.tokens->size;
    for(int i = 0; i<k; i++)
    {
        int top_kth = index_remap[res.tokens->size - 1 - i];
        printf("%s\n", (char *) res.tokens->keys->arr[top_kth]);
    }
    free(index_remap);
    free(mutual_infos);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);

}




