#define _GNU_SOURCE
#include <stdlib.h>
#include <stdlib.h>

int util_sortby_comparefunc(const void *a, const void *b, void *arg)
{
    int adex = * (int *) a;
    int bdex = * (int *) b;
    int *key = (int *) arg;
    return key[adex] - key[bdex];
}

int *util_sortby_remap(int *key, int len)
{
    int *index_remap = malloc(len*sizeof(*index_remap));
    for(int i = 0; i<len; i++)
        index_remap[i] = i;
    qsort_r(index_remap, len, sizeof(*index_remap), &util_sortby_comparefunc, key);
    return index_remap;
}

