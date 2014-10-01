#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxmap.h"
#include "array_list/arr_list.h"

rxmap *rxmap_new()
{
    rxmap *ret = malloc(sizeof(rxmap));
    ret->size = 0;
    ret->words = arr_list_new();
    ret->root = rxnode_new(-1);
    return ret;
}

void rxmap_add(rxmap *t, char *str)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    arr_list_append(&(t->words), cpy);
    rxnode_add(t->root, cpy, strlen(str), t->size);
    t->size++;
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

void rxnode_add(struct rxnode *n, char *suff, int sufflen, int value)
{
    if(!sufflen)
    {
        n->v = value;
        return;
    }
    if(!n->edges)
    {
        n->edges = calloc(ALPH,sizeof(*n->edges));
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
    printf("%c\n", suff[i]);
    printf("%d\n", i);
    e->len = i;
    rxnode_add(e->node, suff+i, sufflen-i, value);
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


