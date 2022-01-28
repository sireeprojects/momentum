#include "common.h"


int main ()
{
   ring_info r = {};
   set_data (&r); // fill data

   char data[360];
   memcpy (data, (char*)&r, 360);

   ring_info copied = {};
   memcpy ((char*)&copied, data, 360);

   print_data (&copied); // print data

   return 0;
}
