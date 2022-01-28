#include <stdio.h>

int main ()
{
   if(geteuid() != 0) {
      printf ("Running as non-root");
   } else {
      printf ("Running as root");
   }
   
   return 0;
}
