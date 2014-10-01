#include <stdio.h>
#include "radix_map/rxmap.h"

int main()
{
    rxmap *index = rxmap_new();
    rxmap_add(index, "bags");
    rxmap_add(index, "sanimals");
    _rxnode_print_inorder(index->root);
}
