#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"

#define PROJECT_ROOT "/home/n/programming/cstuff/analysis"
#define WORDLIST "/data/small"
#define MAXLEN 50



int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "args: wordlist path");
        exit(-1);
    }
    rxmap *map = build_wordlist(argv[1], MAXLEN);
    printf("%d\n", rxmap_get(map, "the"));
    printf("%d\n", rxmap_get(map, "them"));
    printf("%d\n", rxmap_get(map, "then"));
    printf("%d\n", rxmap_get(map, "they"));
    for(int i = 0; i<map->size; i++)
    {
        printf("%s\n", (char *) map->keys->arr[i]);
    }
    printf("%d\n", map->size);
    rxmap_delete(map);
}

