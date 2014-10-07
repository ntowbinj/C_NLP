#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxmap.h"
#include "array_list/arr_list.h"

static void rxnode_splitedge(struct rxedge *e, int len);
static struct rxnode *rxnode_get(struct rxnode *n, char *suff);
static struct rxnode *rxnode_add(struct rxnode *n, char *suff, int sufflen);
static struct rxnode *rxnode_new(int v);

rxmap *rxmap_new()
{
    rxmap *ret = malloc(sizeof(rxmap));
    ret->size = 0;
    ret->keys = arr_list_new();
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
    rxnode_delete(t->root);
    free(t);
}

int rxmap_get(rxmap *map, char *str)
{
    struct rxnode *n = rxnode_get(map->root, str);
    if(n) return n->v;
    else return -1;
}

char *rxmap_revget(rxmap *map, int ind)
{
    return arr_list_get(map->keys, ind);
}

void rxmap_add(rxmap *m, char *str)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    arr_list_append(m->keys, cpy);
    struct rxnode *n = rxnode_add(m->root, cpy, strlen(str));
    n->v = m->size;
    m->size++;
}

int rxmap_addonce(rxmap *m, char *str)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    struct rxnode *n = rxnode_add(m->root, cpy, strlen(str));
    int prev = n->v;
    if(prev == -1)
    {
        arr_list_append(m->keys, cpy);
        n->v = m->size;
        m->size++;
        return m->size-1;
    }
    else{
        free(cpy);
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

void rxnode_init_edges(struct rxnode *n)
{
    n->edges = calloc(ALPH,sizeof(*n->edges));
}

struct rxnode *rxnode_add(struct rxnode *n, char *suff, int sufflen)
{
    if(!sufflen)
    {
        return n;
    }
    if(!n->edges)
    {
        rxnode_init_edges(n);
    }
    if(!n->edges[IND(suff[0])])
    {
        struct rxnode *new = rxnode_new(-1);
        n->edges[IND(suff[0])] = rxedge_new(suff, sufflen, new);
        return new;
    }
    struct rxedge *e = n->edges[IND(suff[0])];
    int i = 0;
    while(i < e->len && i < sufflen && e->label[i] == suff[i])
    {
        ++i;
    }
    if(i<e->len) rxnode_splitedge(e, i);
    return rxnode_add(e->node, suff+i, sufflen-i);
}

void rxnode_splitedge(struct rxedge *e, int len)
{
    struct rxnode *new = rxnode_new(-1);
    struct rxedge *newedge = rxedge_new(e->label+len, e->len-len, e->node);
    e->len = len;
    e->node = new;
    rxnode_init_edges(new);
    new->edges[IND(e->label[len])] = newedge;
}

struct rxnode *rxnode_get(struct rxnode *n, char *suff)
{
    while(*suff != '\0')
    {
        if(!n->edges)
        {
            return NULL;
        }
        struct rxedge *e = n->edges[IND(*suff)];
        if(!e)
        {
            return NULL;
        }
        int i = 0;
        while(i < e->len && e->label[i] == suff[i])
        {
            if(suff[i] == '\0')
            {
                return NULL;
            }
            i++;
        }
        suff = suff+i;
        n = e->node;
    }
    return n;
}


    
void rxnode_print_inorder(struct rxnode *n)
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
                    rxnode_print_inorder(n->edges[i]->node);
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
            rxnode_init_edges(n);
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
        if(i<e->len) rxnode_splitedge(e, i);
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


