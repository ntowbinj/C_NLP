#include <stdlib.h>
#include "arr_list.h"


arr_list *arr_list_new()
{
    arr_list *ret = malloc(sizeof(*ret));
    ret->capacity = 1;
    ret->size = 0;
    ret->arr = malloc(sizeof(*ret->arr)*ret->capacity);
    return ret;
}

void _arr_list_grow(arr_list *list)
{
    list->capacity *= RESIZE;
    list->arr = realloc(list->arr, sizeof(*list->arr)*list->capacity);
}

void _arr_list_shrink(arr_list *list)
{
    list->capacity /= RESIZE;
    list->arr = realloc(list->arr, sizeof(*list->arr)*list->capacity);
}

//value expected to be on heap
void arr_list_append(arr_list *list, void *value)
{
    list->arr[list->size] = value;
    list->size += 1;
    if(list->size == list->capacity)
    {
        _arr_list_grow(list);
    }
}

void *arr_list_pop(arr_list *list)
{
    list->size -= 1;
    void *ret = list->arr[list->size];
    if(list->size < list->capacity/SHRINKFACTOR)
    {
        _arr_list_shrink(list);
    }
    return ret;
}

void *arr_list_get(arr_list *list, int index)
{
    return list->arr[index];
}

void arr_list_set(arr_list *list, int index, void *value)
{
    list->arr[index] = value;
}


void arr_list_delete(arr_list *list)
{
    for(int i = 0; i<list->size; i++)
    {
        free(list->arr[i]);
    }
    free(list->arr);
    free(list);
}


