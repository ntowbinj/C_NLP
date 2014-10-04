#include <stdio.h>
#include <stdlib.h>
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
        int size;
        char **words = tok_words(line, &size);
        for(int i = 0; i<size; i++)
        {
            rxmap_addonce(ret, words[i]);
        }
        free(words);
    }
    fclose(fp);
    return ret;
}

int *tokens_to_indeces(rxmap *map, char **words, int len)
{
    int *ret = malloc(len*sizeof(*ret));
    for(int i = 0; i<len; i++)
    {
        ret[i] = rxmap_addonce(map, words[i]);
    }
    return ret;
}
