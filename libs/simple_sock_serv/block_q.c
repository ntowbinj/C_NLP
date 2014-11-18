#include <pthread.h>
#include "block_q.h"


int block_q_init(block_q *q, int capacity, int *space)
{
    if(pthread_cond_init(&q->full_empty, NULL) < 0)
        return -1;
    if(pthread_mutex_init(&q->put_take, NULL) < 0)
    {
        pthread_cond_destroy(&q->full_empty);
        return -1;
    }
    q->capacity = capacity;
    q->size = 0;
    q->arr = space;
    q->r = q->w = 0;
    return 0;
}

void block_q_destroy(block_q *q)
{
    pthread_cond_destroy(&q->full_empty);
    pthread_mutex_destroy(&q->put_take);
}

void block_q_put(block_q *q, int value)
{
    pthread_mutex_lock(&q->put_take);
    while(q->size == q->capacity)
    {
        pthread_cond_wait(&q->full_empty, &q->put_take);
    }
    q->arr[q->w++] = value;
    q->w %= q->capacity;
    q->size++;
    pthread_cond_signal(&q->full_empty);
    pthread_mutex_unlock(&q->put_take);
}

int block_q_put_noblock(block_q *q, int value)
{
    pthread_mutex_lock(&q->put_take);
    if(q->size == q->capacity)
    {
        pthread_mutex_unlock(&q->put_take);
        return -1;
    }
    q->arr[q->w++] = value;
    q->w %= q->capacity;
    q->size++;
    pthread_cond_signal(&q->full_empty);
    pthread_mutex_unlock(&q->put_take);
    return 0;
}


int block_q_take(block_q *q)
{
    int ret;

    pthread_mutex_lock(&q->put_take);
    while(!q->size)
    {
        pthread_cond_wait(&q->full_empty, &q->put_take);
    }
    ret = q->arr[q->r++];
    q->r %= q->capacity;
    q->size--;
    pthread_cond_signal(&q->full_empty);
    pthread_mutex_unlock(&q->put_take);

    return ret;
}
