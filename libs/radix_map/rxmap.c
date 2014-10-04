#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxmap.h"
#include "array_list/arr_list.h"

static int rxnode_addonce(struct rxnode *n, char *suff, int sufflen, int value, void *data);
static void _rxnode_splitedge(struct rxedge *e, int len);
static int rxnode_get(struct rxnode *n, char *suff);
static void rxnode_add(struct rxnode *n, char *suff, int sufflen, int value, void *data);
static struct rxnode *rxnode_new(int v);

rxmap *rxmap_new()
{
    rxmap *ret = malloc(sizeof(rxmap));
    ret->size = 0;
    ret->keys = arr_list_new();
    ret->data = arr_list_new();
    ret->root = rxnode_new(-1);
    return ret;
}

void rxnode_delete(struct rxnode *root)
{
    struct rxedge **edges = root->edges;
    if(edges)
    {
        for(int i = 0; i<ALPH; i++)
        {
            if(edges[i])
            {
                rxnode_delete(edges[i]->node);
                free(edges[i]);
            }
        }
        free(edges);
    }
    free(root);
}


void rxmap_delete(rxmap *t)
{
    arr_list_delete(t->keys);
    for(int i = 0; i<t->size; i++)
    {
        void *data_i = arr_list_get(t->data, i);
        free(data_i);
    }
    arr_list_delete(t->data);
    rxnode_delete(t->root);
    free(t);
}

int rxmap_get_indx(rxmap *map, char *str)
{
    return rxnode_get(map->root, str);
}

char *rxmap_revget_indx(rxmap *map, int ind)
{
    return arr_list_get(map->keys, ind);
}

char *rxmap_revget_data(rxmap *map, int ind)
{
    return arr_list_get(map->data, ind);
}

// expects data to be on heap, not str
void rxmap_add(rxmap *m, char *str, void *data)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    arr_list_append(m->keys, cpy);
    arr_list_append(m->data, data);
    rxnode_add(m->root, cpy, strlen(str), m->size, data);
    m->size++;
}

int rxmap_addonce(rxmap *m, char *str, void *data)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    int prev = rxnode_addonce(m->root, cpy, strlen(str), m->size, data);
    if(prev == -1)
    {
        arr_list_append(m->keys, cpy);
        arr_list_append(m->data, data);
        m->size++;
        return m->size-1;
    }
    else{
        free(cpy);
        free(data);
        return prev;
    }
}

struct rxedge *rxedge_new(char* label, int len, struct rxnode *n)
{
    struct rxedge *ret;
    ret = malloc(sizeof(*ret));
    ret->label = label;
    ret->len = len;
    ret->node = n;
    return ret;
}


struct rxnode *rxnode_new(int v)
{
    struct rxnode *ret;
    ret = malloc(sizeof(*ret));
    ret->v = v;
    ret->edges = NULL;
    return ret;
}

void _rxnode_init_edges(struct rxnode *n)
{
    n->edges = calloc(ALPH,sizeof(*n->edges));
}

void rxnode_add(struct rxnode *n, char *suff, int sufflen, int value, void *data)
{
    if(!sufflen)
    {
        n->v = value;
        n->data = data;
        return;
    }
    if(!n->edges)
    {
        _rxnode_init_edges(n);
    }
    if(!n->edges[IND(suff[0])])
    {
        struct rxnode *new = rxnode_new(value);
        n->edges[IND(suff[0])] = rxedge_new(suff, sufflen, new);
        return;
    }
    struct rxedge *e = n->edges[IND(suff[0])];
    int i = 0;
    while(i < e->len && i < sufflen && e->label[i] == suff[i])
    {
        ++i;
    }
    if(i<e->len) _rxnode_splitedge(e, i);
    rxnode_add(e->node, suff+i, sufflen-i, value, data);
}

int rxnode_addonce(struct rxnode *n, char *suff, int sufflen, int value, void *data)
{
    if(!sufflen)
    {
        int old = n->v;
        if(old == -1)
        {
            n->v = value;
        }
        return old;
    }
    if(!n->edges)
    {
        _rxnode_init_edges(n);
    }
    if(!n->edges[IND(suff[0])])
    {
        struct rxnode *new = rxnode_new(value);
        n->edges[IND(suff[0])] = rxedge_new(suff, sufflen, new);
        return -1;
    }
    struct rxedge *e = n->edges[IND(suff[0])];
    int i = 0;
    while(i < e->len && i < sufflen && e->label[i] == suff[i])
    {
        ++i;
    }
    if(i<e->len) _rxnode_splitedge(e, i);
    return rxnode_addonce(e->node, suff+i, sufflen-i, value, data);
}



void _rxnode_splitedge(struct rxedge *e, int len)
{
    struct rxnode *new = rxnode_new(-1);
    struct rxedge *newedge = rxedge_new(e->label+len, e->len-len, e->node);
    e->len = len;
    e->node = new;
    _rxnode_init_edges(new);
    new->edges[IND(e->label[len])] = newedge;
}

int rxnode_get(struct rxnode *n, char *suff)
{
    while(*suff != '\0')
    {
        if(!n->edges)
        {
            return -1;
        }
        struct rxedge *e = n->edges[IND(*suff)];
        if(!e)
        {
            return -1;
        }
        int i = 0;
        while(i < e->len && e->label[i] == suff[i])
        {
            if(suff[i] == '\0')
            {
                return -1;
            }
            i++;
        }
        suff = suff+i;
        n = e->node;
    }
    return n->v;
}


    
void _rxnode_print_inorder(struct rxnode *n)
{
    if(n)
    {
        if(n->v > -1)
        {
            printf("%d\n", n->v);
        }
        if(n->edges)
        {
            for(int i = 0; i<ALPH; i++)
            {
                if(n->edges[i])
                {
                    _rxnode_print_inorder(n->edges[i]->node);
                }
            }
        }
    }
}


/*int rxnode_addonce_iter(struct rxnode *n, char *suff, int sufflen, int value)
{
    while(*suff != '\0')
    {
        if(!n->edges)
        {
            _rxnode_init_edges(n);
        }
        if(!n->edges[IND(suff[0])])
        {
            struct rxnode *new = rxnode_new(value);
            n->edges[IND(suff[0])] = rxedge_new(suff, sufflen, new);
            return -1;
        }
        struct rxedge *e = n->edges[IND(suff[0])];
        int i = 0;
        while(i < e->len && i < sufflen && e->label[i] == suff[i])
        {
            ++i;
        }
        if(i<e->len) _rxnode_splitedge(e, i);
        n = e->node;
        suff += i;
        sufflen -= i;
    }
    int old = n->v;
    if(old == -1)
    {
        n->v = value;
    }
    return old;
}*/


