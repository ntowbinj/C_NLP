#include <stdio.h>
#include "radix_map/rxmap.h"

int main()
{
    rxmap *index = rxmap_new();
    rxmap_add(index, "animals");
    rxmap_add(index, "bags");
    rxmap_add(index, "bats");
    rxmap_add(index, "candy");
    rxmap_add(index, "castro");
    printf("%s\n", (char*) arr_list_get(index->words, 2));
    _rxnode_print_inorder(index->root);
    printf("%d\n", rxmap_get(index, "candy"));
    rxmap_delete(index);
}
