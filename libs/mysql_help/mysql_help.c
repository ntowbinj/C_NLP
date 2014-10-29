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

MYSQL_RES *mysql_get_part(MYSQL *conn, int part)
{
    return mysql_query_part(conn, part, MYSQL_SELECT_TEXT);
}

MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query)
{
    // in case the partition number is 10 digits (never)
    char str[strlen(query) + 10];
    sprintf(str, query, part);
    mysql_query(conn, str);
    return mysql_store_result(conn);
}

void mysql_visit_rows(MYSQL *conn, struct mysql_visitor visitor)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int part, rownum;
    part = rownum = 0;
    while(part<NUMPARTS && rownum<visitor.row_count)
    {
        result = mysql_query_part(conn, part, visitor.query);
        part++;
        while((row = mysql_fetch_row(result)) && rownum<visitor.row_count)
        {
            (*visitor.per_row)(row, visitor.arg);
            rownum++;
        }
        mysql_free_result(result);
    }
}
