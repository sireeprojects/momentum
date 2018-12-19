#include <iostream>
#define CALL_MEMBER_FN(object, ptrToMember)  ((object).*(ptrToMember))

using namespace std;

class a 
{
public:

   a(){};

   int afunc(int x) {
      cout << "from afunc : "<< x <<endl;
   }
};

int main () 
{
   a inst_a;
   inst_a.afunc(1);
   int (a::*fptr) (int x) = &a::afunc;
   ((inst_a).*(fptr)) (2);
   CALL_MEMBER_FN(inst_a, fptr)(3);

   a *ptr_a;
   ptr_a->afunc(1);
   int (a::*p_fptr) (int x) = &a::afunc;
   ((*ptr_a).*(p_fptr)) (2);
   CALL_MEMBER_FN(*ptr_a, p_fptr)(3);
}
