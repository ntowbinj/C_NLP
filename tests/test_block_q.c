#include <stdio.h>
#include <string.h>
#include "simple_sock_serv/block_q.h"

block_q q;

void *putter(void *ignore)
{
    for(int i = 0; i<500000; i++)
    {
        block_q_put(&q, i);
    }
    return NULL;
}

void *taker(void *ignore)
{
    for(int i = 0; i<500000; i++)
    {
        block_q_take(&q);
    }
    return NULL;
}


int main()
{
    int size = 6;
    int space[size];
    memset(space, -1, sizeof(space));
    block_q_init(&q, size, space);
    pthread_t put2, put1, tak1, tak2;
    pthread_create(&put1, NULL, &putter, NULL); 
    pthread_create(&put2, NULL, &putter, NULL); 
    pthread_create(&tak1, NULL, &taker, NULL);
    pthread_create(&tak2, NULL, &taker, NULL);
    pthread_join(put1, NULL);
    pthread_join(put2, NULL);
    pthread_join(tak1, NULL);
    pthread_join(tak2, NULL);

    for(int i = 0; i<size*2; i++)
    {
        int wasput = block_q_put_noblock(&q, i);
        printf("%d: %d\n", i, wasput);
    }

    block_q_destroy(&q);
}
