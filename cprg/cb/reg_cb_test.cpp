#include <iostream>
#include <string>
#define CALL_MEMBER_FN(object, ptrToMember)  ((object).*(ptrToMember))
using namespace std;

class proxy;
class adaptor;

struct Event {
   proxy *dev;
   void (proxy::*cbFunc)();
   // void (proxy::*cbFunc)(int fd, proxy *dev);
};

Event events[256];

//---------
// Adaptor
//---------
class adaptor 
{
public:   
   // void (proxy::*cbFunc)();
   // void (proxy::*cbFunc)(int fd, proxy *dev);
   adaptor();
   void add_device (proxy *p);
   void add_fd (int fd, proxy *dev, int t=0);
   int id;
   void test(int fd);
};
//--------
// Proxy
//--------
class proxy 
{
public:
   adaptor *ad; 
   string name;
   proxy(string n) {
      name = n;
   }
   void accept_sock();
   void kick_cb();
};

adaptor::adaptor() 
{
   id=0;
}

void adaptor::add_device (proxy *p) 
{
   add_fd (id, p);
   id++;
}

void adaptor::add_fd (int fd, proxy *dev, int t)
{
   if (t==0)
   {
      events[fd].dev = dev;
      events[fd].cbFunc = &proxy::accept_sock;
   }
   else if (t==1){
      events[fd].dev = dev;
      events[fd].cbFunc = &proxy::kick_cb;
   }
}

void proxy::accept_sock()
{
   cout << "From: " << name << " : accept_sock" << endl;
}

void proxy::kick_cb()
{
   cout << "From: " << name << " : kick_cb" << endl;
}

void adaptor::test(int fd) 
{
   ((*events[fd].dev).*(events[fd].cbFunc))();
}

//-------------
// Main Program
//-------------
int main ()
{
   // create adaptor
   adaptor *a = new adaptor;

   // create proxy devices
   proxy *p0 = new proxy ("p0");
   proxy *p1 = new proxy ("p1");

   // add proxy devices to adaptor
   a->add_device(p0);
   a->add_device(p1);

   a->test(0);
   a->test(1);
   //
   a->add_fd (2, p0, 1);
   a->add_fd (3, p1, 1);
   a->test(2);
   a->test(3);
   //
   a->test(0);
   a->test(1);
}
