#ifndef BUILD
#define BUILD

#include "radix_map/rxmap.h"

rxmap *build_wordlist(char* filename, int);
void build_load_n_tokens(FILE *fp, int count, rxmap *dest, int maxlen);
int *tokens_to_indeces(rxmap *map, char **words, int len);

int *tokens_to_indeces_filtered(rxmap *map, char **words, int len);

int *tokens_to_indeces_length_limited(rxmap *map, char **words, int size, int maxlen);

#endif
