#ifndef MYSQL_HELP
#define MYSQL_HELP

#include<mysql.h>

MYSQL *mysql_start();
void mysql_finish(MYSQL *conn);
MYSQL_RES *mysql_get_part(MYSQL *conn, int part);
MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query);

#endif
