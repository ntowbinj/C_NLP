#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#include "tokenize/tokenize.h"
#include "radix_map/rxmap.h"

#define RATE 1000

int main()
{
    MYSQL *conn;
    conn = mysql_start();
    MYSQL_ROW row;
    char *query = "select text, sub.name from comment partition(p%d)\
                   inner join post \
                   on comment.post_id = post.id \
                   inner join sub on post.sub_id = sub.id;";

    MYSQL_RES *result = mysql_query_part(conn, 91, query);
    int rownum = 0;
    rxmap *submap = rxmap_new();
    if(!result)
    {
        fprintf(stderr, "is NULL\n");
        exit(-1);
    }
    char substr[30];
    while((row = mysql_fetch_row(result)))
    {
        //fprintf(stderr, "%s\n", tok_all_to_lowerc(row[1]));
        strcpy(substr, row[1]);
        tok_all_to_lowerc(substr);
        rxmap_addonce(submap, substr);
        if(!(rownum%RATE)) printf("%s\n", row[1]);
        rownum++;
    }
    mysql_free_result(result);
    for(int i = 0; i < submap->size; i++)
    {
        printf("%d: %s\n", i, (char *) submap->keys->arr[i]);
    }

    mysql_finish(conn);
    printf("%d\n", rxmap_get(submap, "hellofff"));
    rxmap_delete(submap);
    exit(0);
}


