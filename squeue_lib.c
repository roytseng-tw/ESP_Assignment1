#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "squeue_lib.h"

int sq_create(queue **sq, int length) {
    (*sq) = malloc(sizeof(queue));
    (*sq)->front = (*sq)->rear = 0;
    (*sq)->data = malloc(sizeof(void*)*(length)); //circular queue will waste one space
    (*sq)->len = length;
    if((*sq)->data == NULL) {
        printf("Cannot allocate memory.\n");
        return 1;
    }
    pthread_mutex_init( &((*sq)->locker), NULL);
    return 0;
}

int sq_read(queue *sq, void*item) {
    /*dequeue*/
    pthread_mutex_lock( &sq->locker);

    if(sq->front==sq->rear) {
        //printf("queue is empty.\n");
        pthread_mutex_unlock( &sq->locker);
        return -1;
    }
    printf("read.\n");
    item = sq->data[sq->front];
    if(sq->front!=sq->len)
        sq->front ++;
    else
        sq->front = 0;
    pthread_mutex_unlock( &sq->locker);
    return 0;
}

int sq_write(queue *sq, void *item) {
    /*enqueue*/
    pthread_mutex_lock( &sq->locker);
    printf("haha\n");
    if(sq->front==sq->rear+1 || (sq->front==0 && sq->rear==sq->len) ) {
        //printf("queue is full.\n");
        pthread_mutex_unlock( &sq->locker);
        return -1;
    }
    printf("write. %d, %d\n",sq->front, sq->rear);
    sq->data[sq->rear] = item;
    if(sq->rear!=sq->len)
        sq->rear ++;
    else
        sq->rear = 0;
    pthread_mutex_unlock( &sq->locker);
    return 0;
}

int sq_delete(queue **sq, void (*clean)(void* m)) {
    int i;
    //if not empyt
    if((*sq)->front < (*sq)->rear) {
        for(i=(*sq)->front; i<(*sq)->rear; i++)
            clean((*sq)->data[i]);
            //free((*sq)->data[i]->str);
        free((*sq)->data);
    } else if((*sq)->front > (*sq)->rear) {
        for(i=(*sq)->front; i<(*sq)->len; i++)
            clean((*sq)->data[i]);
            //free((*sq)->data[i]->str);
        for(i=0; i<(*sq)->rear; i++)
            clean((*sq)->data[i]);
            //free((*sq)->data[i]->str);
        free((*sq)->data);
    }
    pthread_mutex_destroy(&((*sq)->locker));
    free(*sq);
    free(sq);
    return 0;
}
