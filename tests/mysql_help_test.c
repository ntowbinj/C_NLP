#include<stdio.h>
#include <mysql.h>
#include "mysql_help/mysql_help.h"
#define RATE 10000

int main()
{
    MYSQL *conn;
    conn = mysql_start();
    MYSQL_RES *result = mysql_get_part(conn, 93);
    MYSQL_ROW row;
    int rownum = 0;
    while((row = mysql_fetch_row(result)))
    {
        if(!(rownum%RATE)) printf("%s\n", row[0]);
        rownum++;
    }
    mysql_free_result(result);
    mysql_finish(conn);
}

