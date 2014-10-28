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
    // in case the partition number is 5 digits
    char str[strlen(query) + 5];
    sprintf(str, query, part);
    mysql_query(conn, str);
    return mysql_store_result(conn);
}

void mysql_apply_per_row(
        MYSQL *conn, 
        char *query, 
        int row_count, 
        void (*do_per_row) (MYSQL_ROW, void *), 
        void *arg)
{
    mysql_apply_per_row_from(conn, query, row_count, do_per_row, arg, 0);
}

void mysql_apply_per_partition(
        MYSQL *conn, 
        char *query, 
        int part_count, 
        void (*do_per_row) (MYSQL_ROW, void *), 
        void *arg)
{
    mysql_apply_per_partition_from(conn, query, part_count, do_per_row, arg, 0);
}

void mysql_apply_per_row_from(
        MYSQL *conn,
        char *query,
        int row_count,
        void (*do_per_row) (MYSQL_ROW, void *),
        void *arg,
        int start_part)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int part, rownum;
    part = start_part;
    rownum = 0;
    while(part<NUMPARTS && rownum<row_count)
    {
        result = mysql_query_part(conn, part, query);
        part++;
        while((row = mysql_fetch_row(result)) && rownum<row_count)
        {
            (*do_per_row)(row, arg);
            rownum++;
        }
        mysql_free_result(result);
    }
}

void mysql_apply_per_partition_from(
        MYSQL *conn,
        char *query,
        int part_count,
        void (*do_per_row) (MYSQL_ROW, void *),
        void *arg,
        int start_part)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int part = start_part;
    while(part<NUMPARTS && part<part_count)
    {
        result = mysql_query_part(conn, part, query);
        part++;
        while((row = mysql_fetch_row(result)))
        {
            (*do_per_row)(row, arg);
        }
        mysql_free_result(result);
    }
}

