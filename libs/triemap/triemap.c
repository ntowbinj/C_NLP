#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "triemap.h"
#include "array_list/arr_list.h"

triemap* triemap_new()
{
    triemap *ret = malloc(sizeof(*ret));
    ret->root = trie_new();
    ret->words = arr_list_new();
    ret->size = 0;
    return ret;
}

void triemap_add(triemap* map, char *str)
{
    char *cpy = malloc((strlen(str)+1)*sizeof(*cpy));
    strcpy(cpy, str);
    arr_list_append(map->words, cpy);
    trie_add(map->root, cpy, map->size);
    map->size++;
}

int triemap_get(triemap* map, char *str)
{
    return trie_get_val(map->root, str);
}

char* triemap_revget(triemap* map, int ind)
{
    return arr_list_get(map->words, ind);
}

void triemap_delete(triemap* map)
{
    trie_delete(map->root);
    arr_list_delete(map->words);
    free(map);
}

trie* trie_new()
{
    trie *ret = (trie*) malloc(sizeof(trie));
    ret->v = -1;
    for(int i = 0; i<UNDERC; i++)
        ret->kids[i] = NULL;
    return ret;
}

void trie_delete(trie *root)
{
    if(root){
        for(char c = 'a'; c<='z'; c++)
        {
            trie_delete(root->kids[I(c)]);
        }
        free(root);
    }
}

int trie_add(trie *t, char *word, int v)
{
    if(*word == '\0')
    {
        t->v = v;
        return 1;
    }
    else
    {
        if(!INSIDE(*word)) return 0;
        if(!_trie_haskid(t, *word)) t->kids[I(*word)] = trie_new();
        return trie_add(t->kids[I(*word)], word+1, v);
    }
}

int trie_get_val(trie *t, const char *word) 
{
    return _trie_get_attr(t, word, _trie_ext_val);
}


trie *trie_get(trie *t, const char *word)
{
    if(*word == '\0')
        return t;
    else if (!_trie_haskid(t, *word)) return NULL;
    else return trie_get(t->kids[I(*word)], word+1);
}

/*void trie_reverse_index(trie *t, char** list)
{
    int index = 0;
    _trie_reverse_index_rec(t, 0, &index, list);  
}


void _trie_reverse_index_rec(trie *t, int depth, int *index, char** list)
{
    if(t)
    {
        if(t->v>-1)
        {
            char** current = list+*index;
            *current = malloc((depth+1)*sizeof(char));
            *(*current+depth) = '\0';
            (*index)++;
        }

        for(char c = 'a'; c<='z'; c++)
        {
            if(_trie_haskid(t, c))
            {
                int before = *index;
                _trie_reverse_index_rec(t->kids[I(c)], depth+1, index, list);
                int after = *index;
                for(int i = before; i<after; i++)
                {
                    char *writeptr = *(list+i)+depth;
                    *writeptr = c;
                }
            }
        }
    } 
}*/


/*void trie_make_index(trie *t, int* ind)
{
    if(t)
    {
        if(t->v>-1)
        {
            t->v = *ind;
            (*ind)++;
        }
        for(char c = 'a'; c<='z'; c++)
        {
            if(_trie_haskid(t, c))
                trie_make_index(t->kids[I(c)], ind);
        }
    }
}*/


void trie_print_alph(trie *t, int maxlen)
{
    char buff[maxlen];
    buff[0] = '\0';
    _trie_print_alph_rec(t, buff, 0);
}

int _trie_max_depth(trie *t)
{
    if(t)
    {
        int maxkid = 0;
        for(char c = 'a'; c<='z'; c++)
        {
            if(_trie_haskid(t, c))
            {
                int kiddepth = _trie_max_depth(t->kids[I(c)]);
                if(kiddepth>maxkid) maxkid=kiddepth;
            }
        }
        return maxkid+1;
    }
    else return 0;
}

void _trie_print_alph_rec(trie *t, char *buff, int termdex)
{
    if(t->v>-1)
    {
        printf("%s\n", buff);
    }
    buff[termdex+1] = '\0';
    for(char c = 'a'; c<='z'; c++)
    {
        if(_trie_haskid(t, c))
        {
            buff[termdex] = c;
            _trie_print_alph_rec(t->kids[I(c)], buff, termdex+1);
        }
    }
}

int _trie_ext_val(trie *t) 
{
    return t->v;
}

int _trie_get_attr(trie *t, const char *word, int (*base)(trie*))
{
    if(*word == '\0' && t->v>-1)
        return (*base)(t);
    else if (!_trie_haskid(t, *word)) return 0;
    else return _trie_get_attr(t->kids[I(*word)], word+1, base);
}

int _trie_haskid(trie *t, char c)
{
    if(!INSIDE(c)) return 0;
    return (t->kids[I(c)] != NULL);
}

