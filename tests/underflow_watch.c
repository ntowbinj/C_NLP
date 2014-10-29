#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "args: resources file\n\n");
        exit(1);
    }

    struct raw_resources res = read_raw_resources(argv[1]);

    double **param_vecs = get_param_vecs(
            res.classes->size,
            res.tokens->size,
            res.class_counts,
            res.occurrences_matrix,
            &default_estimator,
            0.1);

    free_raw_resources_arrays(res);

    for(int i = 0; i<10; i++)
    {
        printf("\n%s\n\n", (char *) res.classes->keys->arr[i]);
        for(int j=  9000; j<9010; j++)
        {
            printf("%s: %f\n", (char *) res.tokens->keys->arr[j], param_vecs[i][j]);
        }
    }

    free_param_vecs(res.classes->size, param_vecs);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);


}


