#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "test2.h"

void *test_routine(void *arg) {
    int id = *(int*)arg;
    printf("id=%d.\n",id);
}

int main(int argc, char *argv[]) {
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
