#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#include "tokenize/tokenize.h"

#define RATE 100000

int main()
{
    /*
    MYSQL *conn;
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
    mysql_finish(conn);
*/
    char *line = "man this is ag great sentence! isn't it, holy cow!!! boy oh boy is this a really good, really nice 'n long sentence we've got goin here. sure is goddangit.  sure fuckin is boah constrictors, emerald boahs, sea lions.  gun.oil.github.io. is the best";
    int reps = 100000;
    for(int i = 0; i<reps; i++){
        char *cpy = malloc(sizeof(*cpy)*(1 + strlen(line)));
        strcpy(cpy, line);
        int size;
        char **toks = tok_words(cpy, &size);
        free(cpy);
        free(toks);
    }
}

