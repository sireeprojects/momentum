#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

#define handle_error_en(en, msg) \
      do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace std;

class cputest 
{
public:
    cputest(){};
    ~cputest(){}
    
    std::thread th;
    
    void infinite_loop() {
        int s, j;
        cpu_set_t cpuset;
        pthread_t thread;
        thread = pthread_self();
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
        if (s != 0) {
            handle_error_en(s, "pthread_setaffinity_np");
        }
        while (1) {
            // infinite loop
        }
    }
    
    void start_thread() {
        th = std::thread (&cputest::infinite_loop, this);
        th.detach();
    }
};



int main(int argc, char *argv[])
{
    int cpuAffinity = argc > 1 ? atoi(argv[1]) : -1;

    if (cpuAffinity > -1) {
        cpu_set_t mask;
        int status;
        CPU_ZERO(&mask);
        CPU_SET(cpuAffinity, &mask);
        status = sched_setaffinity(0, sizeof(mask), &mask);
        if (status != 0) {
            perror("sched_setaffinity");
        }
    }
    cputest cpu;
    cpu.start_thread();

    while (1) {
        // infinite loop
    }
}
