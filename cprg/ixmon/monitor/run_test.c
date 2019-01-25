#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *worker(void *args);

int main() {
    int i;
    int nof_threads = 6;
    int thID[nof_threads];
    pthread_t threads[nof_threads];

    // populate thread id for identification
    for (i=0; i<6; i++) {
        thID[i] = i;
    }
    for (i=0; i<6; i++) {
        pthread_create (&threads[i], NULL, &worker, (void*)&thID[i]);
    }
    // wait for all threads to finish
    for (i=0; i<6; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

void *worker(void *args) {
    int id = *((int*)args);
    int ret;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    // pin threads 0-2 to cpu:0
    // pin threads 3-5 to cpu:1
    // if (id<3)
    //     CPU_SET(0, &cpuset);
    // else {
    //     CPU_SET(1, &cpuset);
    // }
        CPU_SET(1, &cpuset);
    // pin the thread 
    ret = pthread_setaffinity_np(
            pthread_self(), sizeof(cpu_set_t), &cpuset); 

   char thread_name[80];
   sprintf(thread_name, "thread_%d", id);
   pthread_setname_np(pthread_self(), thread_name);

    // go to work
    while(1) {
        // sleep(1);
    }
}

