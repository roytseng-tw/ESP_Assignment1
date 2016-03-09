#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void *test_routine(void *arg) {
    int id = *(int*)arg;
    printf("id=%d.\n",id);
}

int main(int argc, char *argv[]) {
    printf("haha\n");
    pthread_t thread_id;
    void *thread_result;
    
    int id = 5;
    pthread_create(&thread_id, NULL, test_routine, &id);
    
    printf("I'm in the main function.\n");
    
    pthread_join(thread_id, &thread_result); 
     
    return 0;
}
