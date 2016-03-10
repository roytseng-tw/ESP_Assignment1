#include <pthread.h>
#include <inttypes.h>

typedef struct {
    int id;
    int src;
    int dst;
    uint64_t acc_time;    //record total queueing time(CPU cycles)
    uint64_t start_time;  //record the queue entering time, changed when entering another queue
    char *str;
} message;

#define DEFAULT_QUEUE_LEN 10
typedef struct {
    int len;                //size of the circular queue
    int front;
    int rear;
    pthread_mutex_t locker; //mutex lock used for synchronization
    message **data;
} queue;

int sq_create(queue **sq, int length);
int sq_write(queue *sq, message *item);
int sq_read(queue *sq, message **item);
int sq_delete(queue *sq);
