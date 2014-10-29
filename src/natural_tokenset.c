#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void natural_per_row(MYSQL_ROW row, void *argptr)
{
    struct natural_arg arg = * (struct natural_arg *) argptr;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces;
    if(arg.counts)
    {
        indeces = tokens_to_indeces_filtered(arg.tokens, toks, size);
        for(int i = 0; i<size; i++)
        {
            if(indeces[i] != -1)
                arg.counts[indeces[i]] ++;
        }
    }
    else
    {
        indeces = tokens_to_indeces_length_limited(arg.tokens, toks, size, MAXLEN);
    }

    free(toks);
    free(indeces);
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "args: num rows, top how many\n\n");
        exit(1);
    }
    rxmap *tokens = rxmap_new();

    struct natural_arg arg =
    {
        .counts = NULL,
        .tokens = tokens
    };

    struct mysql_visitor natural_vis =
    {
        .query = MYSQL_SELECT_TEXT,
        .start_row = 0,
        .row_count = atoi(argv[1]),
        .per_row = &natural_per_row,
        .arg = &arg
    };

    MYSQL *conn = mysql_start();

    mysql_visit_rows(conn, natural_vis);
    int *counts = calloc(tokens->size, sizeof(*counts));
    arg.counts = counts;
    mysql_visit_rows(conn, natural_vis);

    mysql_finish(conn);

    int *index_remap = util_sortby_remap(arg.counts, tokens->size);

    int n = atoi(argv[2]);
    for(int i = 0; i<n; i++)
    {
        int top_nth = index_remap[tokens->size - 1 - i];
        printf("%s\n", (char *) tokens->keys->arr[top_nth]);
    }

    free(counts);
    free(index_remap);
    rxmap_delete(tokens);


}



