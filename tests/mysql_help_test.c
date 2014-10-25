#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"

#define RATE 1000

int main()
{
    MYSQL *conn;
    conn = mysql_start();
    MYSQL_RES *result = mysql_get_part(conn, 93);
    MYSQL_ROW row;
    int rownum = 0;
    while((row = mysql_fetch_row(result)))
    {
        //if(!(rownum%RATE)) printf("%s\n", row[0]);
        rownum++;
    }
    mysql_free_result(result);


    char *query = "select text, sub.name from comment partition(p%d)\
                   inner join post \
                   on comment.post_id = post.id \
                   inner join sub on post.sub_id = sub.id \
                   where sub.name='funny'";

    result = mysql_query_part(conn, 91, query);
    rownum = 0;
    rxmap *submap = rxmap_new();
    if(!result)
    {
        fprintf(stderr, "is NULL\n");
        exit(-1);
    }
    while((row = mysql_fetch_row(result)))
    {
        rxmap_add(submap, row[1]);
        if(!(rownum%RATE)) printf("%s\n", row[1]);
        rownum++;
    }
    mysql_free_result(result);

    mysql_finish(conn);
    rxmap_delete(submap);
}

