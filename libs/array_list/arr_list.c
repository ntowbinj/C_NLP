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

static void _arr_list_resize(arr_list *list, int newcap)
{
    list->capacity = newcap;
    list->arr = realloc(list->arr, sizeof(*list->arr)*list->capacity);
}

//value expected to be on heap
void arr_list_append(arr_list *list, void *value)
{
    list->arr[list->size] = value;
    list->size += 1;
    if(list->size == list->capacity)
    {
        _arr_list_resize(list, list->capacity*RESIZE);
    }
}

/*void arr_list_setsize(arr_list *list, int size)
{
    if(size < list->size)
    {
        list->size = size;
    }
    _arr_list_resize(list, size);
}*/

void *arr_list_pop(arr_list *list)
{
    list->size -= 1;
    void *ret = list->arr[list->size];
    if(list->size < list->capacity/SHRINKFACTOR)
    {
        _arr_list_resize(list, list->capacity/RESIZE);
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
    arr_list_delete_no_free_data(list);
}

void arr_list_delete_no_free_data(arr_list *list)
{
    free(list->arr);
    free(list);
}

