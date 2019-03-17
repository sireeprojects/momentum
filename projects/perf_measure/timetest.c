#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>

struct timespec diff(struct timespec start, struct timespec end);
void *worker(void *param);

int main(){
    pthread_t pt1;
    pthread_t pt2;
    pthread_create(&pt1, NULL, worker, NULL);
    pthread_create(&pt2, NULL, worker, NULL);
    pthread_join(pt1,NULL);
    pthread_join(pt2,NULL);
    return 0;
}

struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    /* how is this calculation done
     * why the -1 in the if block 
     */
    if ((end.tv_nsec - start.tv_nsec) < 0) { 
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

void *worker(void *param) {
    struct timespec start, end;
    uint32_t val;

    /* record time before engaging the CPU */ 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    /* engage the CPU by doing some computations */
    int i,j;
    for (i=0; i<2; i++) {
        for (j=0; j<1000000000; j++) {
            val = val % 1000000;
        }
    }
    /* record time after end of thread*/ 
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    printf("Time Taken: %d.%d sec\n", diff(start,end).tv_sec, diff(start,end).tv_nsec);
}
