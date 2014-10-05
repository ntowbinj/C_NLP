#include <stdio.h>
#include "radix_map/rxmap.h"

int main()
{
    rxmap *index = rxmap_new();
    rxmap_add(index, "animals", NULL);
    rxmap_add(index, "bags", NULL);
    rxmap_add(index, "bats", NULL);
    rxmap_add(index, "candycane", NULL);
    rxmap_add(index, "candy", NULL);
    rxmap_add(index, "castro", NULL);
    rxnode_print_inorder(index->root);
    printf("castro: %d\n", rxmap_get_indx(index, "castro"));
    rxmap_delete(index);
}
