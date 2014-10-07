#ifndef TRIE_H
#define TRIE_H
#define MAXLEN 20
#define UNDERC 26
#define I(x) x - 'a'
#define INSIDE(x) (x >= 'a' && x <= 'z')
#include "array_list/arr_list.h"

typedef struct trie{
    int v;

    struct trie* kids[UNDERC];
} trie;

typedef struct triemap{
    trie *root;
    int size;
    arr_list *words;
} triemap;


trie* trie_new();
void triemap_add(triemap* map, char *str);
int triemap_get(triemap* map, char *str);
char* triemap_revget_indx(triemap* map, int ind);
triemap* triemap_new();
void triemap_delete(triemap* map);
void trie_delete(trie *);
int _trie_haskid(trie *, char);
int trie_add(trie *, char *, int);
int _trie_ext_val(trie *) ;
int trie_get_val(trie *t, const char *) ;
int _trie_get_attr(trie *, const char *, int (*)(trie*));
trie *get(trie *, const char *);
void _trie_print_alph_rec(trie *, char *, int);
void trie_print_alph(trie *, int maxlen);
void trie_make_index(trie *, int*);
int _trie_max_depth(trie *t);
void _trie_reverse_index_rec(trie *t, int depth, int *index, char** list);
void trie_reverse_index(trie *t, char** list);
#endif
void triemap_addonce(triemap* map, char *str);
int trie_addonce(trie *t, char *word, int v);
