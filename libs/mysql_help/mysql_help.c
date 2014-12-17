#include "connection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mysql_help.h"


MYSQL *mysql_start()
{
    MYSQL *conn = mysql_init(NULL);
    if(!(mysql_real_connect(conn, HOST, USER, PSSWD, DBNAME, PORT, UNIX_SOCKET, FLAG)))
    {
        fprintf(stderr, "\nerror: %s [%d] \n", mysql_error(conn), mysql_errno(conn));
        exit(-1);
    }
    return conn;
}

void mysql_finish(MYSQL *conn)
{
    mysql_close(conn);
    mysql_library_end();
}

char *mysql_default_nth(int n)
{
    char *query = malloc(1024 * sizeof(*query));
    sprintf(query, MYSQL_SELECT_TEXT_AND_CLASS, n);
    return query;
}


void mysql_visit_rows(MYSQL *conn, struct mysql_visitor visitor)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char *query_str;
    int rownum = 0;
    for(int n = 0; rownum<visitor.row_count; n++)
    {
        if(!(query_str = (*visitor.nth_query)(n))) // NULL signals end
            break;
        printf("%s\n", query_str);
        mysql_query(conn, query_str);
        result = mysql_store_result(conn);

        while((row = mysql_fetch_row(result)) && rownum++<visitor.row_count)
            (*visitor.per_row)(row, visitor.arg);

        mysql_free_result(result);
        free(query_str);
    }
}
