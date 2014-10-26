#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"
#include "util/util.h"

#include "config.h"

MYSQL *conn;

void build(int **subvectors, rxmap *subs, rxmap *words, int count)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char *query = "select text, sub.name from comment partition(p%d)\
                   inner join post \
                   on comment.post_id = post.id \
                   inner join sub on post.sub_id = sub.id;";

    int part, rownum;
    part = rownum = 0;
    char substr[30];
    while(part<NUMPARTS && rownum<count)
    {
        result = mysql_query_part(conn, part, query);
        part++;
        while((row = mysql_fetch_row(result)) && rownum<count)
        {
            int size;
            strcpy(substr, row[1]);
            tok_all_to_lowerc(substr);
            int subdex = rxmap_get(subs, substr);
            if(subdex != -1)
            {
                char **toks = tok_words(row[0], &size);
                int *indeces = tokens_to_indeces_filtered(words, toks, size);
                for(int i = 0; i<size; i++)
                {
                    if(indeces[i] != -1)
                    {
                        subvectors[subdex][indeces[i]]++;
                    }
                }
                free(toks);
                free(indeces);
            }
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




int main(int argc, char *argv[])
{
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

    int *subvectors[subs->size];
    for(int i = 0; i<subs->size; i++)
        subvectors[i] = calloc(words->size, sizeof(int));

    conn = mysql_start();

    build(subvectors, subs, words, count);

    int wordc = 40;
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

