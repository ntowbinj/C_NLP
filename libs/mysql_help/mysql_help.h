#ifndef MYSQL_HELP
#define MYSQL_HELP


#include<mysql.h>

#define MYSQL_SELECT_TEXT "select text from comment partition (p%d)"
#define MYSQL_SELECT_TEXT_AND_CLASS "select text, sub.name from comment partition(p%d) inner join post on comment.post_id = post.id inner join sub on post.sub_id = sub.id;"

typedef void (*mysql_per_row_func) (MYSQL_ROW, void *);

struct mysql_visitor
{
    char *query;
    int start_row; // currently ignored
    int row_count;
    mysql_per_row_func per_row;
    int which_half;
    void *arg;
};

MYSQL *mysql_start();

void mysql_finish(MYSQL *conn);

MYSQL_RES *mysql_get_part(MYSQL *conn, int part);

MYSQL_RES *mysql_query_part(MYSQL *conn, int part, char *query);

// non-zero mysql_visitor.start_row not currently supported
void mysql_visit_rows(MYSQL *conn, struct mysql_visitor);

#endif
