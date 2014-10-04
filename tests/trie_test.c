#include <stdio.h>
#include "triemap/triemap.h"

int main()
{
    triemap *index = triemap_new();
    triemap_add(index, "animals");
    triemap_add(index, "bags");
    triemap_add(index, "bats");
    triemap_add(index, "candy");
    triemap_add(index, "castro");
    printf("%s\n", (char*) arr_list_get(index->words, 2));
    trie_print_alph(index->root, 100);
    printf("%d\n", triemap_get(index, "candy"));
    printf("%s\n", triemap_revget_indx(index, 3));
    triemap_delete(index);
    //triemap_delete(index);
}

