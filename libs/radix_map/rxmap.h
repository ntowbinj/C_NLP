#ifndef RXMAP
#define RXMAP
#include "array_list/arr_list.h"

#define ALPH 26
#define IND(i) (i - 'a')

struct rxedge;

typedef struct
{
    int size;
    struct rxnode *root;
    arr_list *keys;
} rxmap;

struct rxnode
{
    int v;
    void *data;
    struct rxedge **edges;
};

struct rxedge
{
    char *label;
    int len;
    struct rxnode *node;
};


rxmap *rxmap_new();
void rxmap_add(rxmap *t, char *str);
void rxnode_print_inorder(struct rxnode *n);
void rxmap_delete(rxmap *t);
void rxmap_delete_no_free_data(rxmap *t);
int rxmap_get(rxmap *map, char *str);
char *rxmap_revget(rxmap *map, int ind);
int rxmap_addonce(rxmap *m, char *str);

#endif





