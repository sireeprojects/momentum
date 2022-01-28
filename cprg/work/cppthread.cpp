#include <iostream>
#include <string>
#include <thread>

using namespace std;

class X {
public:
   string n;

   X(string name) {
      n = name;
   }
   void task() {
      cout << "thread task " << n << endl;
   }

   void operator()() {
      cout << "thread operator" << endl;
   }
};

int main () {
   X x("x");
   X y("y");
   // thread t (x);
   thread t1 (&X::task, x);
   thread t2 (&X::task, y);
   t2.join();
   t1.join();
}
