#define _GNU_SOURCE
#include <sched.h>  // sched_setaffinity
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *infinite_loop(void*arg) {
    int s;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    while (1){} // sleep(1);
}

int main() {
    // cpu_set_t mask;
    // int status;
    // CPU_ZERO(&mask);
    // CPU_SET(3, &mask);
    // status = sched_setaffinity(0, sizeof(mask), &mask);
    // 
    // if (status != 0) {
    //     perror("sched_setaffinity");
    // }
    //
    int s;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    pthread_t t1;
    pthread_create (&t1, NULL, &infinite_loop, (void*)0);
    pthread_join (t1, NULL);
}
