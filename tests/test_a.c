/*
 *
 * demonstrates that implementation of rxmap is faster and more memory efficient than triemap
 * 
 */

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "triemap/triemap.h"
#include "tokenize/tokenize.h"

#define MAP rxmap
#define MAP_get rxmap_get
#define MAP_addonce rxmap_addonce
#define MAP_revget rxmap_revget
#define MAP_new rxmap_new
#define MAP_delete rxmap_delete

void small()
{
    char *strstack = "how about some of this typing, it's pretty fun to just type a bunch of stuff.  awesome";
    int size;
    char *str = malloc(sizeof(*str)*(strlen(strstack)+1));
    strcpy(str, strstack);
    MAP *map = MAP_new();
    char **toks = tok_words(str, &size);
    for(int i = 0; i<size; i++)
    {
        char* w = *(toks+i);
        MAP_addonce(map, w);
    }
    for(int i = 0; i<map->size; i++)
    {
        char *word = MAP_revget(map, i);
        printf("%s:\t\t%d\n", word, MAP_get(map, word));
    }
    free(toks);
    free(str);
    MAP_delete(map);
}

void large(int lim){
    MYSQL *conn;
    conn = mysql_start();
    MYSQL_RES *result = mysql_get_part(conn, 93);
    MYSQL_ROW row;
    int rownum = 0;
    MAP *map = MAP_new();
    int count = 0;
    while((row = mysql_fetch_row(result)) && count<lim)
    {
        count++;
        int size;
        char **toks = tok_words(row[0], &size);
        for(int i = 0; i<size; i++)
        {
            char* w = *(toks+i);
            MAP_addonce(map, w);
        }
        free(toks);
        rownum++;
    }
    for(int i = 0; i<map->size; i++)
    {
        char *word = MAP_revget(map, i);
        printf("%s:\t%d\n", word, MAP_get(map, word));
    }
    MAP_delete(map);
    mysql_free_result(result);
    mysql_finish(conn);
}

int main(int argc, char *argv[])
{
    if(!argc)
    {
        fprintf(stderr, "wrong args\n. give number");
    }
    large(atoi(argv[1]));
}


