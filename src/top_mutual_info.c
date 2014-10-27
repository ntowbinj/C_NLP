#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "array_list/arr_list.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"

#include "config.h"

struct bernoulli_arg
{
    int *zero_slate;
    int *present_totals;
    rxmap *words;
};

struct freq_arg
{
    int *occurrences;
    rxmap *words;
};

MYSQL *conn;

void per_row_freq(MYSQL_ROW row, void *arguments)
{
    struct freq_arg arg = * (struct freq_arg *) arguments;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(arg.words, toks, size);
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1)
        {
            arg.occurrences[indeces[i]]++;
        }
    }
    free(toks);
    free(indeces);
}

void per_row_bernoulli(MYSQL_ROW row, void *arguments)
{
    struct bernoulli_arg arg = * (struct bernoulli_arg *) arguments;
    int size;
    char **toks = tok_words(row[0], &size);
    int *indeces = tokens_to_indeces_filtered(arg.words, toks, size);
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1 && !arg.zero_slate[indeces[i]])
            arg.zero_slate[indeces[i]] = 1;
    }

    // loop through again, and wherever zero_slate
    // is non-zero, incrememt present_totals once, then
    // set zero_slate to be 0 there so that it's
    // not row_counted again, there by row_counting boolean 
    // zero_slate, not frequency, and 'cleaning'
    // zero_slate[]
    for(int i = 0; i<size; i++)
    {
        if(indeces[i] != -1)
        {
            if(arg.zero_slate[indeces[i]])
            {
                arg.present_totals[indeces[i]]++;
                arg.zero_slate[indeces[i]] = 0;
            }
        }
    }
    free(toks);
    free(indeces);
}


// occurrences should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// occurrences[i] will have the total number of times
// word i appeared.
void freq_count(int *occurrences, rxmap *words, int row_count)
{
    struct freq_arg arg = {.occurrences = occurrences, .words = words};
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_freq, &arg);
}

// present_total should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// present_total[i] will have the number of rows
// in which word i was present
void bernoulli_count(int *present_totals, rxmap *words, int row_count)
{
    int zero_slate[words->size];
    struct bernoulli_arg arg = 
    {
        .zero_slate = zero_slate,
        .present_totals = present_totals,
        .words = words
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_bernoulli, &arg);
}




int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        fprintf(stderr, "args: wordlist path, partition count, word count\n");
        exit(-1);
    }

    int rowc = atoi(argv[2]);
    int wordc = atoi(argv[3]);
    printf("path: %s\n", argv[1]);
    rxmap *words = build_wordlist(argv[1], MAXLEN);
    printf("SIZE: %d\n", words->size);
    int *occurrences = calloc(words->size, sizeof(*occurrences));

    conn = mysql_start();

    freq_count(occurrences, words, rowc);
    //bernoulli_count(occurrences, words, rowc);

    int *index_remap = util_sortby_remap(occurrences, words->size);
    for(int i = 0; i<wordc && i<words->size; i++)
    {
        printf("%s\n", (char *) words->keys->arr[index_remap[words->size - 1 - i]]);
    }

    rxmap_delete(words);
    free(occurrences);
    free(index_remap);

    mysql_finish(conn);

}

