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
    arr_list *data;
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
void rxmap_add(rxmap *t, char *str, void *data);
void rxnode_print_inorder(struct rxnode *n);
void rxmap_delete(rxmap *t);
void rxmap_delete_no_free_data(rxmap *t);
int rxmap_get_indx(rxmap *map, char *str);
void *rxmap_get_data(rxmap *map, char *str);
char *rxmap_revget_data(rxmap *map, int ind);
char *rxmap_revget_indx(rxmap *map, int ind);
int rxmap_addonce(rxmap *m, char *str, void *data);

#endif





