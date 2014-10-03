#include <stdlib.h>
#include <string.h> 
#include "tokenize.h"


char **tok_words(char *line, int *size)
{
    int s = OUT;
    int wordc = 0;
    char **words = malloc(strlen(line) * sizeof(*words));
    while(*line != '\0')
    {
        if(!ALPH_INSIDE(*line))
        {
            int shift = 'A' - 'a';
            if(ALPH_INSIDE(*line - shift))
                *line -= shift;
            else
                *line = ' ';
        }
        if(ALPH_INSIDE(*line) && s == OUT)
        {
            s = IN;
            words[wordc] = line;
            wordc++;
        }
        else if(!ALPH_INSIDE(*line) && s == IN)
        {
            s = OUT;
            *line = '\0';
        }
        line++;
    }
    *size = wordc;
    return words;
}
    



