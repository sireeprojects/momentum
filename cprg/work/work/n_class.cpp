#include <iostream>
using namespace std;

class testclass 
{
public:
   static int cnt;
   testclass(){
      cout << "created=" << cnt << endl;
      cnt++;
   };
};

int testclass::cnt=0;

int main()
{
   testclass *t;
   t = new testclass[5];
   return 0;
}
