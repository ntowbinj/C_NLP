#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"
#include "counters/counters.h"
#include "classifier/classifier.h"
#include "mysql_help/mysql_help.h"

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        fprintf(stderr, "args: classes path, tokens path, size of training set, dest file\n\n");
        exit(1);
    }

    struct mysql_visitor visitor = 
    {
        .nth_query = &mysql_default_nth,
        .row_count = atoi(argv[3]),
        .per_row = NULL,
        .arg = NULL
    };

    struct build_params params = 
    {
        .classes_filepath = argv[1],
        .tokens_filepath = argv[2]
    };

    struct raw_resources res = build_raw_resources(params, visitor, MULTINOM); //MULTINOM hardcoded
    store_raw_resources(res, argv[4]);
    free_raw_resources_arrays(res);
    rxmap_delete(res.classes);
    rxmap_delete(res.tokens);
}



