#include <stdio.h>
#include <string.h>
#include "mysql_help/mysql_help.h"
#include "radix_map/rxmap.h"
#include "tokenize/tokenize.h"
#include "build.h"

rxmap *build_wordlist(char* filename, int maxlen)
{
    FILE *fp = fopen(filename, "r");
    char line[maxlen];
    rxmap *ret = rxmap_new();
    while(fgets(line, maxlen, fp))
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


