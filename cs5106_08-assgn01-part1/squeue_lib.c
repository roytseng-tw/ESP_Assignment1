#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "squeue_lib.h"
#include "rdtsc.h"

/*allocate queue memory*/
int sq_create(queue **sq, int length) {
    (*sq) = malloc(sizeof(queue));
    (*sq)->front = (*sq)->rear = 0;
    (*sq)->data = malloc(sizeof(message*)*(length)); //circular queue will waste one space
    (*sq)->len = length;
    if((*sq)->data == NULL) {
        printf("Cannot allocate memory.\n");
        return 1;
    }
    pthread_mutex_init( &((*sq)->locker), NULL);
    return 0;
}

/*dequeue*/
int sq_read(queue *sq, message **item) {
    pthread_mutex_lock( &(sq->locker) );
    //printf("read %p\n", sq);
    if(sq->front==sq->rear) {
        //printf("queue is empty.\n");
        pthread_mutex_unlock( &(sq->locker) );
        return -1;
    }
    *item = sq->data[sq->front];
    (*item)->acc_time = rdtsc() - (*item)->start_time;
    if(sq->front!=sq->len)
        sq->front ++;
    else
        sq->front = 0;
    //printf("after read: %d, %d.\n",sq->front, sq->rear);
    pthread_mutex_unlock( &(sq->locker) );
    
    return 0;
}

/*enqueue*/
int sq_write(queue *sq, message *item) {    
    pthread_mutex_lock( &sq->locker);
    //printf("write %p\n", sq);
    if(sq->front==sq->rear+1 || (sq->front==0 && sq->rear==sq->len) ) {
        //printf("queue is full.\n");
        pthread_mutex_unlock( &sq->locker);
        return -1;
    }
    item->start_time = rdtsc();
    sq->data[sq->rear] = item;
    if(sq->rear!=sq->len)
        sq->rear ++;
    else
        sq->rear = 0;
    //printf("after write: %d, %d.\n",sq->front, sq->rear);
    pthread_mutex_unlock( &sq->locker);
    return 0;
}

/*free queue memory*/
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
    pthread_mutex_destroy(&(sq->locker));
    free(sq);
    return 0;
}
