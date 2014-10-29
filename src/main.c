#include <stdio.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build/build.h"

#include "config.h"


int main()
{
    printf("hello\n");
    rxmap *map = build_tokenlist(PROJECT_ROOT""WORDLIST, MAXLEN);
    printf("%d\n", rxmap_get(map, "smart"));
    printf("%d\n", map->size);
    rxmap_delete(map);
}
