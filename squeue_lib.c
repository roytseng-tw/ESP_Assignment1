#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"squeue_lib.h"

int sq_create(queue *sq) {
    sq_create(sq, DEFAULT_QUEUE_LEN);
    return 0;
}

int sq_create(queue *sq, int length) {
    sq = (queue*) malloc(sizeof(queue));
    sq->front = sq->rear = 0;
    sq->data = (void**) malloc(sizeof(void*)*(length)); //circular queue will waste one space
    sq->len = length;
    if(sq->data == NULL)
        printf("Cannot allocate memory.\n");
        return 1;
    pthread_mutex_init(&sq->locker, NULL);
    return 0;
}

int sq_write(queue *sq, void *item) {
    /*enqueue*/
    pthread_mutex_lock(&sq->locker);
    if(sq->front==sq->rear+1 || sq->front==0 && sq->rear==sq->len) {
        printf("queue is full.\n");
        return -1;
    }
    sq->data[sq->rear] = item;
    if(sq->rear!=sq->len)
        sq->rear ++;
    else
        sq->rear = 0;
    pthread_mutex_unlock(&sq->locker);
    return 0;
}

int sq_read(queue *sq, void *item) {
    /*dequeue*/
    pthread_mutex_lock(&sq->locker);
    if(sq->front==sq->rear) {
        printf("queue is empty.\n");
        return -1;
    }
    item = sq->data[sq->front];
    if(sq->front!=sq->len)
        sq->front ++;
    else
        sq->front = 0;
    pthread_mutex_unlock(&sq->locker);
    return 0
}

int sq_delete(queue *sq) {
    int i;
    //if not empyt
    if(sq->front < sq->rear) {
        for(i=sq->front; i<sq->rear; i++)
            free(sq->data[i]->str);
        free(sq->data);
    } else if(sq->front > sq->rear) {
        for(i=sq->front; i<sq->len; i++)
            free(sq->data[i]->str);
        for(i=0; i<sq->rear; i++)
            free(sq->data[i]->str);
        free(sq->data);
    }
    pthread_mutex_destroy(&sq->locker);
    free(sq);
    return 0;
}
