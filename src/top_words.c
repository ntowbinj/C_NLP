#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"

#include "config.h"

MYSQL *conn;

void build(int *occurences, rxmap *words, int count)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int part, rownum;
    part = rownum = 0;
    while(part<NUMPARTS && rownum<count)
    {
        result = mysql_get_part(conn, part);
        part++;
        while((row = mysql_fetch_row(result)) && rownum<count)
        {
            int size;
            char **toks = tok_words(row[0], &size);
            int *indeces = tokens_to_indeces_filtered(words, toks, size);
            for(int i = 0; i<size; i++)
            {
                if(indeces[i] != -1)
                {
                    occurences[indeces[i]]++;
                }
            }
            free(toks);
            free(indeces);
            rownum++;
        }
        mysql_free_result(result);
    }
}

int maxindex(int *arr, int len)
{
    int maxdex = -1;
    if(len)
    {
        int max = arr[0];
        for(int i = 0; i<len; i++)
        {
            if(arr[i] > max)
            {
                max = arr[i];
                maxdex = i;
            }
        }
    }
    return maxdex;
}

int comparefunc(const void *a, const void *b, void *arg)
{
    int adex = * (int *) a;
    int bdex = * (int *) b;
    int *key = (int *) arg;
    return key[adex] - key[bdex];
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
    int *occurences = calloc(words->size, sizeof(*occurences));

    conn = mysql_start();

    build(occurences, words, rowc);
    /*for(int i = 0; i<words->size; i++)
        printf("%d, ", occurences[i]);
    printf("\n");*/

    int *index_remap = malloc(words->size*sizeof(*occurences));
    for(int i = 0; i<words->size; i++)
        index_remap[i] = i;

    qsort_r(index_remap, words->size, sizeof(*index_remap), &comparefunc, occurences);
    for(int i = 0; i<wordc && i<words->size; i++)
    {
        printf("%s\n", (char *) words->keys->arr[index_remap[words->size - 1 - i]]);
    }

    rxmap_delete(words);
    free(occurences);
    free(index_remap);

    mysql_finish(conn);

}


