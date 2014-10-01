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
    arr_list_delete(t->words);
    rxnode_delete(t->root);
    free(t);
}

int rxmap_get(rxmap *map, char *str)
{
    return rxnode_get(map->root, str);
}

void rxmap_add(rxmap *m, char *str)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    arr_list_append(m->words, cpy);
    rxnode_add(m->root, cpy, strlen(str), m->size);
    m->size++;
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

void rxnode_add(struct rxnode *n, char *suff, int sufflen, int value)
{
    if(!sufflen)
    {
        n->v = value;
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
        printf("new edge at %c\n", suff[0]);
        return;
    }
    struct rxedge *e = n->edges[IND(suff[0])];
    int i = 0;
    while(i < e->len && i < sufflen && e->label[i] == suff[i])
    {
        ++i;
    }
    //printf("%c\n", suff[i]);
    //printf("%d\n", i);
    _rxnode_splitedge(e, i);
    rxnode_add(e->node, suff+i, sufflen-i, value);
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
            printf("one\n");
            return -1;
        }
        struct rxedge *e = n->edges[IND(*suff)];
        if(!e)
        {
            printf("%s\n", suff);
            printf("two\n");
            return -1;
        }
        int i = 0;
        while(i < e->len && e->label[i] == suff[i])
        {
            if(suff[i] == '\0')
            {
                printf("three\n");
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


