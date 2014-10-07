#ifndef ARR_LIST
#define ARR_LIST
#define RESIZE 2
#define SHRINKFACTOR 3

typedef struct arr_list
{
    int size;
    int capacity;
    void** arr;
} arr_list;

arr_list *arr_list_new();
void arr_list_append(arr_list *list, void *value);
void *arr_list_get(arr_list *list, int index);
void arr_list_delete(arr_list *list);
void arr_list_delete_no_free_data(arr_list *list);
void *arr_list_pop(arr_list *list);
void arr_list_set(arr_list *list, int index, void *value);
//void arr_list_setsize(arr_list *list, int size);
#endif
