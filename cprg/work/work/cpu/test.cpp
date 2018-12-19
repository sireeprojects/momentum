// #define _GNU_SOURCE
#include <iostream>
#include <sched.h>  // sched_setaffinity
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

using namespace std;


void *infinite_loop(void*)
{
   int s, j;
   cpu_set_t cpuset;
   pthread_t thread;
    
   thread = pthread_self();
    
   /* Set affinity mask to include CPUs 0 to 7 */
    
   CPU_ZERO(&cpuset);
   CPU_SET(10, &cpuset);
    
   s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
   // if (s != 0) {
          // handle_error_en(s, "pthread_setaffinity_np");
   // }
   while (1)
   {
   }
}

int main(int argc, char *argv[])
{
  int cpuAffinity = argc > 1 ? atoi(argv[1]) : -1;

  if (cpuAffinity > -1)
  {
      cpu_set_t mask;
      int status;

      cout << endl << "CPU Affinity Setting= " << cpuAffinity;

      CPU_ZERO(&mask);
      CPU_SET(cpuAffinity, &mask);
      status = sched_setaffinity(0, sizeof(mask), &mask);
      if (status != 0)
      {
          perror("sched_setaffinity");
      }
  }

   pthread_t t1;
   pthread_create (&t1, NULL, &infinite_loop, (void*)0);
   pthread_detach (t1);


   while (1)
   {
   }
    // ... your program ...
}
