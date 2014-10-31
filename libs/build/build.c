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

void build_load_n_tokens(FILE *fp, int count, rxmap *dest, int maxlen)
{
    char line[maxlen];
    for(int i = 0; i<count; i++)
    {
        fgets(line, maxlen, fp);
        int pos = strlen(line) -1;
        if(line[pos] == '\n')
        {
            line[pos] = '\0';
        }
        int size; // assumed to be exactly 1, ignore
        char **words = tok_words(line, &size);
        rxmap_addonce(dest, words[0]);
        free(words);
    }
}

int *tokens_to_indeces_filtered(rxmap *map, char **words, int size)
{
    int *ret = malloc(size*sizeof(*ret));
    for(int i = 0; i<size; i++)
    {
        ret[i] = rxmap_get(map, words[i]);
    }
    return ret;
}

int *tokens_to_indeces(rxmap *map, char **words, int size)
{
    int *ret = malloc(size*sizeof(*ret));
    for(int i = 0; i<size; i++)
    {
        ret[i] = rxmap_addonce(map, words[i]);
    }
    return ret;
}

int *tokens_to_indeces_length_limited(rxmap *map, char **words, int size, int maxlen)
{
    int *ret = malloc(size*sizeof(*ret));
    for(int i = 0; i<size; i++)
    {
        if(strlen(words[i]) < maxlen)
            ret[i] = rxmap_addonce(map, words[i]);
        else
            ret[i] = -1;
    }
    return ret;
}
