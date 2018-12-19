#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;
typedef void * (*THREADFUNCPTR)(void *);

class X
{
public:
   string n;

   X(string name)
   {
      n = name;
   }
   void *task(void *arg)
   {
      cout << "thread task " << n << endl;
   }

};

int main ()
{
   X x("x");
   pthread_t pt;
   pthread_create (&pt, NULL, (THREADFUNCPTR) &X::task, &x);
   // pthread_create (&pt, NULL, (void* (X::*)(void*)) &X::task, &x);
   // pthread_create (&pt, NULL, &X::task, &x);
   pthread_join (pt,NULL);
}
