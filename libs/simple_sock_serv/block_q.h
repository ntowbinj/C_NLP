#ifndef BLOCK_Q_H
#define BLOCK_Q_H
#include <pthread.h>

typedef struct block_q 
{
    int capacity;
    int size;
    int *arr;
    pthread_cond_t full_empty;
    pthread_mutex_t put_take;
    int r;
    int w;
} block_q;


int block_q_init(block_q *q, int capacity, int *space);

void block_q_destroy(block_q *q);

void block_q_put(block_q *q, int value);

int block_q_put_noblock(block_q *q, int value);

int block_q_take(block_q *q);

#endif
