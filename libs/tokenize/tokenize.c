#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include "tokenize.h"
#define SHIFT ('A' - 'a')
#define WHITE(c) (c == ' ' || c == '\n' || c == '\0')



char *tok_all_to_lowerc(char *line)
{
    char *ret = line;
    while(*line != '\0')
    {
        if(*line >= 'A' && *line <= 'Z')
        {
            *line -= SHIFT;
        }
        else if(*line < 'a' || *line > 'z')
        {
            *line = 'a' + (((int) *line)&15);
            //*line = 'a';
        }
        line++;
    }
    return ret;
}
        
char **tok_words(char *line, int *size)
{
    char *ahead, *behind, endpt;
    ahead = behind = line;
    int wordc = 0;
    char **words = malloc(strlen(line) * sizeof(*words));
    do
    {
        while(WHITE(*behind) && *behind != '\0')
        {
            behind++;
        }
        if(*behind == '\0') break;
        ahead = behind;
        while(!WHITE(*ahead) && *ahead != '\0')
        {
            if(*ahead == '.' || *ahead == ',' || *ahead == '!' || *ahead == '-')
            {
                *ahead = ' ';
                break;
            }
            ahead++;
        }
        char *walk_ahead, *walk_behind;
        walk_ahead = walk_behind = behind;
        endpt = *ahead;
        while(walk_ahead < ahead)
        {
            if(ALPH_INSIDE(*walk_ahead))
            {
                *(walk_behind++) = *walk_ahead;
            }
            else if(ALPH_INSIDE(*walk_ahead - SHIFT))
            {
                *(walk_behind++) = *walk_ahead - SHIFT;
            }
            else if(*walk_ahead != '\'')
            {
                break;
            }
            walk_ahead++;
        }
        if(walk_ahead == ahead)
        {
            //don't add if empty
            if(walk_behind - behind)
            {
               words[wordc++] = behind;
            }
            *walk_behind = '\0';
        }
        behind = ahead+1;
    } while(endpt != '\0');
    *size = wordc;
    return words;
}

/*char **tok_words_naive(char *line, int *size)
{
    int s = OUT;
    int wordc = 0;
    char **words = malloc(strlen(line) * sizeof(*words));
    while(*line != '\0')
    {
        if(!ALPH_INSIDE(*line))
        {
            if(ALPH_INSIDE(*line - SHIFT))
                *line -= SHIFT;
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
}*/
