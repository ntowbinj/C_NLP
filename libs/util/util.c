#define _GNU_SOURCE
#include <stdlib.h>
#include <stdlib.h>

static int util_sortby_comparefunc(const void *a, const void *b, void *arg)
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

static int util_sortby_comparefunc_dbl(const void *a, const void *b, void *arg)
{
    int adex = * (int *) a;
    int bdex = * (int *) b;
    double *key = (double *) arg;
    if(key[adex] - key[bdex] < 0) return -1;
    if(key[adex] - key[bdex] > 0) return 1;
    return 0;
}

int *util_sortby_remap_dbl(double *key, int len)
{
    int *index_remap = malloc(len*sizeof(*index_remap));
    for(int i = 0; i<len; i++)
        index_remap[i] = i;
    qsort_r(index_remap, len, sizeof(*index_remap), &util_sortby_comparefunc_dbl, key);
    return index_remap;
}




