#include<pthread.h>

#define DEFAULT_QUEUE_LEN 10
typedef struct {
    int len;
    int front;
    int rear;
    pthread_mutex_t locker;
    void **data;
} queue;

int sq_create(queue *sq, int length);
int sq_write(queue *sq, void *item);
int sq_read(queue *sq, void *item);
int sq_delete(queue *sq, void *(*clean)(void *));
