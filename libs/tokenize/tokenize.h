#ifndef TOKENIZE
#define TOKENIZE

#define ALPH_INSIDE(x) ((x) >= 'a' && (x) <= 'z')

enum tok_state {IN, OUT};

char **tok_words(char *line, int *size);
int pure_alph(char *word);
void p(char *word);

#endif
