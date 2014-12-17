#ifndef MYSQL_HELP
#define MYSQL_HELP


#include<mysql.h>

#define MYSQL_SELECT_TEXT "select text from comment partition (p%d)"
#define MYSQL_SELECT_TEXT_AND_CLASS "select text, sub.name from comment partition(p%d) inner join post on comment.post_id = post.id inner join sub on post.sub_id = sub.id;"

typedef void (*mysql_per_row_func) (MYSQL_ROW, void *);
typedef char* (*mysql_nth_query) (int);

struct mysql_visitor
{

    mysql_nth_query nth_query;
    int row_count;
    mysql_per_row_func per_row;
    void *arg;
};

MYSQL *mysql_start();

void mysql_finish(MYSQL *conn);

void mysql_visit_rows(MYSQL *conn, struct mysql_visitor);

char *mysql_default_nth(int n);

#endif
