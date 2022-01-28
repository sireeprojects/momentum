#include <iostream>
#include <cmath>


using namespace std;

int main() 
{
   int len = 4000;

   int nbufs;
   nbufs = (ceil) ((float) (len+48)/4096);
   cout << "Num bufs= " << nbufs << endl;

   return 0;
}
