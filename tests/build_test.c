#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "tokenize/tokenize.h"
#include "radix_map/rxmap.h"
#include "build/build.h"

#define RATE 100000

int main()
{
    char *joined = "afkl; adi dj kalb jkl;di jkfsl abra cadarkb avada kadarv hewlitt packard and the sor. sorcer. sorcerer's. sorcerer's st. sorcerer's sto. stttttthello pi sui woo pig soo pig pig mcdonald had3mfarms";
    char copy[strlen(joined) + 1];
    strcpy(copy, joined);
    int size;
    char **words = tok_words(copy, &size);
    rxmap *map = rxmap_new();
    int *indeces = tokens_to_indeces(map, words, size);
    for(int i = 0; i<size; i++)
    {
        printf("%d: %s\n", indeces[i], (char *) map->keys->arr[indeces[i]]);
    }
    rxmap_delete(map);
    free(indeces);
    free(words);
}


