#include <stdio.h>
#include "../arr_list.h"

int main()
{
    arr_list ar = arr_list_new();
    int len = 10;
    char *words[10] = {"pigs", "boats", "dogs", "white", "people", "farris wheels", "aminals", "eight", "nine", "ten"};
    for(int i = 0; i<len; i++)
    {
        arr_list_append(&ar, words[i]);
    }
    for(int i = 0; i<len; i++)
    {
        arr_list_set(&ar, i, words[len - 1 - i]);
    }
    for(int i = 0; i<len; i++)
    {
        char *cur = arr_list_pop(&ar);
        printf("%s\n", cur);
        printf("%d\n", ar.capacity);
    }
    arr_list_delete(&ar);
}
