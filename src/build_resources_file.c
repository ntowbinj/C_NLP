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
    if(argc < 5)
    {
        fprintf(stderr, "args: classes path, tokens path, size of training set, dest file\n\n");
        exit(1);
    }
    struct build_params params = 
    {
        .classes_filepath = argv[1],
        .tokens_filepath = argv[2],
        .training_size = atoi(argv[3])
    };

    struct raw_resources res = build_raw_resources(params);
    store_raw_resources(res, argv[4]);
    free_raw_resources_arrays(res);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}



