// g++ -std=c++11 cb_stdfunc.cpp

#include <iostream>
#include <functional>

using namespace std;

class a 
{
public:
   a(){};
   int afunc(int x) 
   {
      cout << "from afunc : "<< x <<endl;
   }
};

int foo (int x, int y) 
{
   cout << "From foo : " << x << " " << y << endl;
}

int main ()
{
   // calling the free function
   foo (1,2);
   std::function<int(int,int)> call_foo1 = &foo;
   call_foo1(3,4);

   // calling class function
   // object call
   a b; 
   std::function<int (a&,int)> call_afunc = &a::afunc;
   call_afunc (b, 5);
   
   // pointer call
   std::function<int(a*, int)> call_afunc_ptr = &a::afunc;
   call_afunc_ptr (&b, 6);
}
