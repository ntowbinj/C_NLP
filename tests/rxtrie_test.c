#include <stdio.h>
#include "radix_map/rxmap.h"

int main()
{
    rxmap *index = rxmap_new();
    rxmap_add(index, "animals");
    rxmap_add(index, "bags");
    rxmap_add(index, "bats");
    rxmap_add(index, "candycane");
    rxmap_add(index, "candy");
    rxmap_add(index, "castro");
    _rxnode_print_inorder(index->root);
    printf("castro: %d\n", rxmap_get(index, "castro"));
    rxmap_delete(index);
}
