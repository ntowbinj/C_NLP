#ifndef MYSQL_HELP
#define MYSQL_HELP


#include<mysql.h>

#define MYSQL_SELECT_TEXT "select text from comment partition (p%d)"

MYSQL *mysql_start();
void mysql_finish(MYSQL *conn);
MYSQL_RES *mysql_get_part(MYSQL *conn, int part);
MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query);
void mysql_apply_per_row(MYSQL *conn, char *query, int row_count, void (*do_per_row) (MYSQL_ROW, void *), void *arg);
#endif
