/*
 *
 * show words that get used twice or more in a row
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "array_list/arr_list.h"
#include "tokenize/tokenize.h"
#include "build/build.h"

#include "config.h"

void print_dups(char **toks, int *indeces, int len)
{
    for(int i = 0; i<len-1; i++)
    {
        if(indeces[i] == indeces[i+1])
        {
            printf("%s\n", toks[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "wrong args.\ngive number.\n");
        exit(1);
    }
    int count = atoi(argv[1]);
    MYSQL *conn;
    conn = mysql_start();
    MYSQL_RES *result = mysql_get_part(conn, 93);
    MYSQL_ROW row;
    rxmap *map = rxmap_new();
    int part, rownum;
    part = rownum = 0;
    while(part<NUMPARTS && rownum<count)
    {
        part++;
        while((row = mysql_fetch_row(result)) && rownum<count)
        {
            int size;
            char **toks = tok_words(row[0], &size);
            int *indeces = tokens_to_indeces(map, toks, size);
            print_dups(toks, indeces, size);
            free(toks);
            free(indeces);
            rownum++;
        }
        mysql_free_result(result);
    }
    rxmap_delete(map);
    mysql_finish(conn);
}


