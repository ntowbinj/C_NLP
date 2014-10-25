#include <stdio.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"

#define PROJECT_ROOT "/home/n/programming/cstuff/analysis"
#define WORDLIST "/data/small"
#define MAXLEN 50



int main()
{
    printf("hello\n");
    rxmap *map = build_wordlist(PROJECT_ROOT""WORDLIST, MAXLEN);
    printf("%d\n", rxmap_get(map, "will"));
    printf("%d\n", rxmap_get(map, "willing"));
    printf("%d\n", map->size);
    rxmap_delete(map);
}

