#ifndef MYSQL_HELP
#define MYSQL_HELP


#include<mysql.h>

#define MYSQL_SELECT_TEXT "select text from comment partition (p%d)"
#define MYSQL_SELECT_TEXT_AND_CLASS "select text, sub.name from comment partition(p%d) inner join post on comment.post_id = post.id inner join sub on post.sub_id = sub.id;"

typedef void (*mysql_per_row_func) (MYSQL_ROW, void *);

MYSQL *mysql_start();

void mysql_finish(MYSQL *conn);

MYSQL_RES *mysql_get_part(MYSQL *conn, int part);

MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query);

void mysql_apply_per_row(
        MYSQL *conn,
        char *query,
        int row_count,
        mysql_per_row_func per_row,
        void *arg);

void mysql_apply_per_row_from(
        MYSQL *conn,
        char *query,
        int row_count,
        mysql_per_row_func per_row,
        void *arg,
        int start_part);

void mysql_apply_per_partition(
        MYSQL *conn,
        char *query,
        int part_count,
        mysql_per_row_func per_row,
        void *arg);

void mysql_apply_per_partition_from(
        MYSQL *conn,
        char *query,
        int part_count,
        mysql_per_row_func per_row,
        void *arg,
        int start_part);
#endif
