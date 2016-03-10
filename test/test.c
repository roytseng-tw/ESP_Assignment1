#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "test2.h"

static inline uint32_t __iter_div_u64_rem(uint64_t dividend, uint32_t divisor, uint64_t *remainder)
{
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
static inline void timespec_add_ns(struct timespec *a, uint64_t ns)
{
      a->tv_sec += __iter_div_u64_rem(a->tv_nsec + ns, NSEC_PER_SEC, &ns);
        a->tv_nsec = ns;
}

void *test_routine(void *arg) {
    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);

    int i;
    for(i=0; i<10; i++) {
        printf("%d",i);
        int id = *(int*)arg;
        printf("id=%d.\n",id);
        next.tv_sec += 1;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, 0);
    }
}

int main(int argc, char *argv[]) {

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);
    printf("%lld %ld\n",(long long)next.tv_sec, next.tv_nsec);

    pthread_t thread_id;
    void *thread_result;
    
    int id = 5;
    pthread_create(&thread_id, NULL, test_routine, &id);
    
    printf("I'm in the main function.\n");
    
    pthread_join(thread_id, &thread_result);
    
    test_struct* bbb;
    create(&bbb);
    printf("%d\n", bbb->a);

    add(bbb);
    printf("%d\n", bbb->a);

    test_struct bla = *bbb;
    printf("%d\n", (&bla)->a);


    return 0;
}
