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

struct bernoulli_arg_flat
{
    int *zero_slate;
    int *present_totals;
    rxmap *words;
};

struct multinom_arg_flat
{
    int *occurrences;
    rxmap *words;
};

struct bernoulli_arg_matrix
{
    int *zero_slate;
    int **present_totals_matrix;
    int *sub_counts;
    rxmap *subs;
    rxmap *words;
};

struct multinom_arg_matrix
{
    int **occurrences_matrix;
    int *sub_counts;
    rxmap *subs;
    rxmap *words;
};


MYSQL *conn;

static inline void per_row_multinom_flat(MYSQL_ROW row, void *arguments)
{
    struct multinom_arg_flat arg = * (struct multinom_arg_flat *) arguments;
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

static inline void per_row_bernoulli_flat(MYSQL_ROW row, void *arguments)
{
    struct bernoulli_arg_flat arg = * (struct bernoulli_arg_flat *) arguments;
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
    // zero_slate, not multinomuency, and 'cleaning'
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

static inline void per_row_multinom_matrix(MYSQL_ROW row, void *arguments)
{
    struct multinom_arg_matrix arg = * (struct multinom_arg_matrix *) arguments;
    tok_all_to_lowerc(row[1]);
    int subdex = rxmap_get(arg.subs, row[1]);
    if(subdex != -1)
    {
        arg.sub_counts[subdex]++;
        struct multinom_arg_flat flat_arg = 
        {
            .occurrences = arg.occurrences_matrix[subdex],
            .words = arg.words
        };
        per_row_multinom_flat(row, &flat_arg);
    }
}

static inline void per_row_bernoulli_matrix(MYSQL_ROW row, void *arguments)
{
    struct bernoulli_arg_matrix arg = * (struct bernoulli_arg_matrix *) arguments;
    tok_all_to_lowerc(row[1]);
    int subdex = rxmap_get(arg.subs, row[1]);
    if(subdex != -1)
    {
        arg.sub_counts[subdex]++;
        struct bernoulli_arg_flat flat_arg = 
        {
            .zero_slate = arg.zero_slate,
            .present_totals = arg.present_totals_matrix[subdex],
            .words = arg.words
        };
        per_row_bernoulli_flat(row, &flat_arg);
    }
}



// occurrences should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// occurrences[i] will have the total number of times
// word i appeared.
void multinom_flat(int *occurrences, rxmap *words, int row_count)
{
    struct multinom_arg_flat arg = {.occurrences = occurrences, .words = words};
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_multinom_flat, &arg);
}

// present_total should be a zero'd array of length
// words->size.
// this function parses count rows, and afterward
// present_total[i] will have the number of rows
// in which word i was present
void bernoulli_flat(int *present_totals, rxmap *words, int row_count)
{
    int zero_slate[words->size];
    memset(zero_slate, 0, sizeof(*zero_slate)*words->size);
    struct bernoulli_arg_flat arg = 
    {
        .zero_slate = zero_slate,
        .present_totals = present_totals,
        .words = words
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT, row_count, &per_row_bernoulli_flat, &arg);
}

void bernoulli_matrix(int *sub_counts, int **present_totals_matrix, rxmap *subs, rxmap *words, int row_count)
{
    int zero_slate[words->size];
    memset(zero_slate, 0, sizeof(*zero_slate)*words->size);
    struct bernoulli_arg_matrix arg = 
    {
        .zero_slate = zero_slate,
        .sub_counts = sub_counts,
        .present_totals_matrix = present_totals_matrix,
        .words = words,
        .subs = subs
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT_AND_SUB, row_count, &per_row_bernoulli_matrix, &arg);
}

void multinom_matrix(int *sub_counts, int **occurrences_matrix, rxmap *subs, rxmap *words, int row_count)
{
    struct multinom_arg_matrix arg = 
    {
        .occurrences_matrix = occurrences_matrix, 
        .sub_counts = sub_counts,
        .words = words,
        .subs = subs
    };
    mysql_apply_per_row(conn, MYSQL_SELECT_TEXT_AND_SUB, row_count, &per_row_multinom_matrix, &arg);
}



int main(int argc, char *argv[])
{
    /*if(argc < 4)
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

    //multinom_flat(occurrences, words, rowc);
    bernoulli_flat(occurrences, words, rowc);

    int *index_remap = util_sortby_remap(occurrences, words->size);
    for(int i = 0; i<wordc && i<words->size; i++)
    {
        printf("%s\n", (char *) words->keys->arr[index_remap[words->size - 1 - i]]);
    }

    rxmap_delete(words);
    free(occurrences);
    free(index_remap);

    mysql_finish(conn);*/

    if(argc < 2)
    {
        fprintf(stderr, "wrong args.\ngive number.\n");
        exit(1);
    }

    int count = atoi(argv[1]);
    rxmap *subs = build_wordlist(PROJECT_ROOT""SUBLIST, MAXLEN);
    rxmap *words = build_wordlist(PROJECT_ROOT""WORDLIST, MAXLEN);
    for(int i = 0; i<subs->size; i++)
    {
        printf("%d: %s\n", i, (char *) subs->keys->arr[i]);
    }

    int sub_counts[subs->size];
    memset(sub_counts, 0, subs->size*sizeof(*sub_counts));
    int *subvectors[subs->size];
    for(int i = 0; i<subs->size; i++)
        subvectors[i] = calloc(words->size, sizeof(int));

    conn = mysql_start();

    multinom_matrix(sub_counts, subvectors, subs, words, count);
    for(int i = 0; i<subs->size; i++)
    {
        printf("sub %d: %d\n", i, sub_counts[i]);
    }

    int wordc = 50;
    for(int i = 0; i<subs->size; i++)
    {
        char *sub = subs->keys->arr[i];
        int *index_remap = util_sortby_remap(subvectors[i], words->size);
        printf("sub: %s\n", sub);
        for(int i = 0; i<wordc && i<words->size; i++)
            printf("%s, ", (char *) words->keys->arr[index_remap[words->size - 1 - i]]);
        printf("\n\n\n");
        free(index_remap);
    }

    for(int i = 0; i<subs->size; i++)
        free(subvectors[i]);

    rxmap_delete(subs);
    rxmap_delete(words);

    mysql_finish(conn);

}

