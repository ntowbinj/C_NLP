#include <stdio.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "config.h"

rxmap *wordlist_from_file()
{
    FILE *fp = fopen(PROJECT_ROOT""WORDLIST, "r");
    char line[MAXLEN];
    rxmap *ret = rxmap_new();
    while(fgets(line, MAXLEN, fp))
    {
        int pos = strlen(line) -1;
        if(line[pos] == '\n')
        {
            line[pos] = '\0';
        }
        rxmap_addonce(ret, line);
    }
    fclose(fp);
    return ret;
}

int main()
{
    printf("hello\n");
    rxmap *map = wordlist_from_file();
    printf("%d\n", rxmap_get(map, "smart"));
    printf("%d\n", map->size);
}
