#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

pthread_t worker_pt;
pthread_t monitor_pt;

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void pclock(char *msg, clockid_t cid) {
    struct timespec ts;
    printf("%s", msg);
    if (clock_gettime(cid, &ts) == -1)
        handle_error("clock_gettime");
    printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
}

static void *worker(void *arg) {
    printf("Subthread starting infinite loop\n");
    clockid_t cid;
    int s = pthread_getcpuclockid(pthread_self(), &cid);
    if (s != 0)
        handle_error_en(s, "pthread_getcpuclockid");

    // sleep does not count in the clock
    
    long j;
    double k=1.345;
    for (j = 0; j < 90000000; j++)
         k = k*0.832;
        //getppid();
    pclock("worker: ", cid);
}

static void *monitor(void *arg) {
    int s;
    int i;
    clockid_t cid;
    sleep(5);
    // for (i=0; i<10; i++) {
        // s = pthread_getcpuclockid(worker_pt, &cid);
        // pclock("worker: ", cid);
        // sleep(1);
    // }
}

int main() {
    pthread_create(&worker_pt, NULL, worker, NULL);
    pthread_create(&monitor_pt, NULL, monitor, NULL);
    pthread_join(worker_pt, NULL);
    // pthread_join(monitor_pt, NULL);
    return 0;
}
