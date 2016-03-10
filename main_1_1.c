#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include "squeue_lib.h"
#include "message.h"

#define SENDER_NUM 4
#define RECEIVER_NUM 3
#define BASE_PERIOD 1000000 //nano-seconds
#define MAX_STR_LEN 80

typedef struct {
    queue *bus_in_q;
    queue **bus_out_q;
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

#define MAX_MSG_CNT 20000
static int msn = 0; //message serial number, also used as a counter
pthread_rwlock_t msn_rwlock; // = PTHREAD_RWLOCK_INITIALIZER; (another way to initialize.)

const int period_multiplier[] = {12, 22, 24, 26, 28, 15, 17, 19};
const int thread_priority[] = {90, 94, 95, 96, 97, 91, 92, 93};

void *deamon_routine(void *arg) {
    deamon_arg *arg2 = (deamon_arg *)arg;
    struct timespec next;
    uint64_t period = BASE_PERIOD * period_multiplier[0];
    clock_gettime(CLOCK_MONOTONIC, &next);

    message *m;

    pthread_rwlock_rdlock(&msn_rwlock);
    while(msn < MAX_MSG_CNT) {
        pthread_rwlock_unlock(&msn_rwlock);
        printf("%d\n",msn);
        while(sq_read(arg2->bus_in_q, m) != -1) {
            //sq_write(arg2->bus_out_q[m->dst], m);
            //check dropped
        }
        //printf("deamon: end of reading.\n");
        timespec_add_ns(&next, period);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    }
    pthread_rwlock_unlock(&msn_rwlock);

    while(sq_read(arg2->bus_in_q, m) != -1) {
        sq_write(arg2->bus_out_q[m->dst], m);
        //check dropped
    }
}

void *sender_routine(void *arg) {
    sender_arg* arg2 = (sender_arg*) arg;
    struct timespec next;
    uint64_t period = BASE_PERIOD * period_multiplier[arg2->sender_id];
    clock_gettime(CLOCK_MONOTONIC, &next);
    
    pthread_rwlock_rdlock(&msn_rwlock);
    while(msn < MAX_MSG_CNT) {
        pthread_rwlock_unlock(&msn_rwlock);
        int n = (rand() % 4) + 1; //number of messages to send
        int i;
        for(i=0; i<n; i++) {
            message m;
            pthread_rwlock_wrlock(&msn_rwlock);
            if(msn >= MAX_MSG_CNT) {
                pthread_rwlock_unlock(&msn_rwlock);
                return;
            }
            printf("%d\n", msn);
            m.id = msn++; 
            pthread_rwlock_unlock(&msn_rwlock);
            m.src = arg2->sender_id;
            m.dst = rand() % RECEIVER_NUM;
            m.acc_time = 0;
            m.str = (char*) malloc(sizeof(char) * ((rand()%71)+10));
            sq_write(arg2->bus_in_q, &m);
        }
        timespec_add_ns(&next, period);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    }
    pthread_rwlock_unlock(&msn_rwlock);
}

void *receiver_routine(void* arg) {
    receiver_arg* arg2 = (receiver_arg*) arg;
    struct timespec next;
    uint64_t period = BASE_PERIOD * period_multiplier[5 + arg2->receiver_id];
    clock_gettime(CLOCK_MONOTONIC, &next);

    message m;

    pthread_rwlock_rdlock(&msn_rwlock);
    while(msn < MAX_MSG_CNT) { 
        pthread_rwlock_unlock(&msn_rwlock);
        
        while(sq_read(arg2->bus_out_q, &m) != -1) {
            //record queue time
        }

        timespec_add_ns(&next, period);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    }
    pthread_rwlock_unlock(&msn_rwlock);

    while(sq_read(arg2->bus_out_q, &m) != -1) {
        //record queue time
    }
}


int main(int argc, char *argv[]) {

    queue *bus_in_q, *bus_out_q[RECEIVER_NUM];

    pthread_t bus_deamon, sender[4], receiver[3];
    //pthread_attr_t th_attr;
    void *thread_result[5];
    int prio_index = 0;

    sq_create(&bus_in_q, DEFAULT_QUEUE_LEN);
    int i;
    for (i=0; i<RECEIVER_NUM; i++) {
        sq_create(&(bus_out_q[i]), DEFAULT_QUEUE_LEN);
    }

    pthread_rwlock_init(&msn_rwlock, NULL);
    
    srand(time(NULL));

    /*deamon_arg arg;
    arg.bus_in_q = bus_in_q;
    arg.bus_out_q = (queue**) malloc(sizeof(queue*)*RECEIVER_NUM);
    for(i=0; i<RECEIVER_NUM; i++)
        arg.bus_out_q[i] = bus_out_q[i];
    pthread_create(&bus_deamon, NULL, deamon_routine, &arg);
    pthread_setschedprio(bus_deamon, thread_priority[prio_index++]);*/

    
    for(i=0; i<SENDER_NUM; i++) {
        sender_arg arg;
        arg.sender_id = i;
        arg.bus_in_q = bus_in_q;
        pthread_create(&sender[i], NULL, sender_routine, &arg);
        pthread_setschedprio(sender[i], thread_priority[prio_index++]);
    }

    for(i=0; i<RECEIVER_NUM; i++) {
        receiver_arg arg;
        arg.receiver_id = i;
        arg.bus_out_q = bus_out_q[i];
        pthread_create(&receiver[i], NULL, receiver_routine, &arg);
        pthread_setschedprio(receiver[i], thread_priority[prio_index++]);
    }

    pthread_join(bus_deamon, NULL);
    for(i=0; i<SENDER_NUM; i++) {
        pthread_join(sender[i], NULL);
    }
    for(i=0; i<RECEIVER_NUM; i++) {
        pthread_join(receiver[i], NULL);
    }
    
    return 0;
}
