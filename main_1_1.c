#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include"squeue_lib.h"

#define SENDER_NUM 4
#define RECEIVER_NUM 3
#define BASE_PERIOD 1000000 //nano-seconds
#define MAX_STR_LEN 80
typedef struct {
    int id;
    int src;
    int dst;
    int qtime;
    char *str;
} message;

typedef struct {
    queue *bus_in_q;
    queue *bus_out_q[];
} deamon_arg;

typedef struct {
    int sender_id;
    queue *bus_in_q;
} sender_arg;

typedef struct {
    int receiver_id;
    queue *bus_out_q;
} receiver_arg;

// source: https://gist.github.com/BinaryPrison/1112092
static inline uint32_t __iter_div_u64_rem(uint64_t dividend, uint32_t divisor, uint64_t *remainder) {
    uint32_t ret = 0;
    while (dividend >= divisor) {
    /* The following asm() prevents the compiler from
       optimising this loop into a modulo operation.  */
        asm("" : "+rm"(dividend));
        dividend -= divisor;
        ret++;
    }
    *remainder = dividend;
    return ret;
}
#define NSEC_PER_SEC  1000000000L
static inline void timespec_add_ns(struct timespec *a, uint64_t ns) {
    a->tv_sec += __iter_div_u64_rem(a->tv_nsec + ns, NSEC_PER_SEC, &ns);
    a->tv_nsec = ns;
}

static int msn = 0; //message serial number
pthread_mutex_t msn_locker;
static int msg_cnt = 0; //created messages counter 

void *deamon_routine(void *arg) {
    deamon_arg* arg = (deamon_arg*) arg;
    message m;
    if(sq_read(arg->bus_in_q, &m) != -1)
        sq_write(arg->bus_out_q[m.dst], &m);
}

void *sender_routine(void *arg) {
    sender_arg* arg = (sender_arg*) arg;
    struct timespec next;
    uint64_t period = BASE_PERIOD * period_multiplier[arg->sender_id];
    clock_gettime(CLOCK_MONOTONIC, &next);

    while(/*undo*/) {
        int n = (rand() % 4) + 1; //number of messages to send
        int i;
        for(i=0; i<n; i++) {
            message m;
            pthread_mutex_lock(&msn_locker);
            m.id = msn++; 
            pthread_mutex_unlock(&msn_locker);
            m.src = arg->sender_id;
            m.dst = rand() % RECEIVER_NUM;
            m.qtime = 0;
            m.str = (char*) malloc(sizeof(char) * ((rand()%71)+10));
            sq_write(arg->bus_in_q, &m);
        }
        timespec_add_ns(&next, period);
        clock_nanosleep(ClOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    }
    
}

void *receiver_routine() {
    receiver_arg* arg = (receiver_arg*) arg;
    int i;
    for(i=0; i<n; i++) {
        message m;
        sq_read(arg->bus_out_q, &m);
    }
}


int main(int argc, char *argv[]) {

    queue bus_in_q, bus_out_q[RECEIVER_NUM];

    const int period_multiplier[] = {12, 22, 24, 26, 28, 15, 17, 19};
    const int thread_priority[] = {90, 94, 95, 96, 97, 91, 92, 93};
    pthread_t bus_deamon, sender[4], receiver[3];
    //pthread_attr_t th_attr;
    void *thread_result[5];
    int prio_index = 0;

    sq_create(bus_in_q);
    int i;
    for (i=0; i<RECEIVER_NUM; i++) {
        sq_create(bus_out_q[i]);
    }

    pthread_mutex_init(&msn_locker, NULL);
    
    srand(time(NULL));

    deamon_arg arg;
    arg.bus_in_q = &bus_in_q;
    arg.bus_out_q = (queue**) malloc(sizeof(queue*)*RECEIVER_NUM);
    for(i=0; i<RECEIVER_NUM; i++)
        arg.bus_out_q[i] = &bus_out_q[i];
    pthread_create(&bus_deamon, NULL, deamon_routine, &arg);
    pthread_setschedprio(bus_deamon, thread_priority[prio_index++]);

    
    for(i=0; i<SENDER_NUM; i++) {
        sender_arg arg;
        arg.sender_id = i;
        arg.bus_in_q = &bus_in_q;
        pthread_create(&sender[i], NULL, sender_routine, &arg);
        pthread_setschedprio(sender[i], thread_priority[prio_index++]);
    }

    for(i=0; i<RECEIVER_NUM; i++) {
        receiver_arg arg;
        arg.receiver_id = i;
        arg.bus_out_q = &bus_out_q[i];
        pthread_create(&receiver[i], NULL, receiver_routine, &arg);
        pthread_setschedprio(receiver[i], thread_priority[prio_index++]);
    }

    return 0;
}
