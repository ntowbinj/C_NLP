#ifndef BUILD
#define BUILD

#include "radix_map/rxmap.h"

rxmap *build_wordlist(char* filename, int);
int *tokens_to_indeces(rxmap *map, char **words, int len);

int *tokens_to_indeces_filtered(rxmap *map, char **words, int len);

#endif
