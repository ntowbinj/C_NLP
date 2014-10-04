#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#include "tokenize/tokenize.h"

#define RATE 100000

int main()
{
    /*MYSQL *conn;
    conn = mysql_start();
    MYSQL_RES *result = mysql_get_part(conn, 93);
    MYSQL_ROW row;
    int rownum = 0;
    while((row = mysql_fetch_row(result)))
    {
        int size;
        if(!(rownum%RATE)){
            printf("%s\n", row[0]);
            char **toks = tok_words(row[0], &size);
            for(int i = 0; i<size; i++)
            {
                char* w = *(toks+i);
                printf("%s\n", w);
            }
            free(toks);
            printf("\n\n");
        }
        rownum++;
    }
    mysql_free_result(result);
    mysql_finish(conn);*/

    char *line = "It's just so MIC\n\n";
    char *cpy = malloc(sizeof(*cpy)*(1 + strlen(line)));
    strcpy(cpy, line);
    int size;
    printf("%s\n", line);
    char **toks = tok_words(cpy, &size);
    for(int i = 0; i<size; i++)
    {
        printf("%s\n", toks[i]);
    }
    free(cpy);
    free(toks);
}

