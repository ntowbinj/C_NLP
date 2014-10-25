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
    char str[100];
    sprintf(str, SELECT_FORMAT, part);
    mysql_query(conn, str);
    return mysql_store_result(conn);
}

MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query)
{
    char str[strlen(query) + 5];
    sprintf(str, query, part);
    mysql_query(conn, str);
    return mysql_store_result(conn);
}
