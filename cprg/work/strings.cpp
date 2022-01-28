#include <iostream>
#include <string>
#include <sstream>

using namespace std;
int main ()
{
   string a;
   char ab[6];
   stringstream os;

   for (int i=0; i<7; i++)
      ab[i] = 0;

   for (int i=0; i<7; i++)
   os << std::hex << (int) ab[i];

   cout << os.str() << endl;
   return 0;
}
