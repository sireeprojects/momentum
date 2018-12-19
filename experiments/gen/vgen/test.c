#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fvm.h"


int main ()
{
   struct fvm_s *spec;
   uint32_t i=0;
   spec = (struct fvm_s*) malloc (sizeof (struct fvm_s));
   memset (spec, 0, sizeof (struct fvm_s));

   // incremental
   spec->start=1;
   spec->type = increment;
   spec->step = 10;
   spec->init_value = 0;
   for (i=0; i<10; i++)
   printf ("Generated value [%d] = %d \n", i, get_next_value (spec));
   printf ("\n");

   // decrement 
   spec->start=1;
   spec->type = decrement;
   spec->step = 1;
   spec->init_value = 0xffffffff;
   for (i=0; i<10; i++)
   printf ("Generated value [%d] = %x \n", i, get_next_value (spec));
   printf ("\n");

   // fixed 
   spec->start=1;
   spec->type = fixed;
   spec->value = 100;
   for (i=0; i<10; i++)
   printf ("Generated value [%d] = %d \n", i, get_next_value (spec));
   printf ("\n");
   
   // random
   spec->start=1;
   spec->type = rnd;
   for (i=0; i<10; i++)
   printf ("Generated value [%d] = %d \n", i, get_next_value (spec));
   printf ("\n");

   return 0;
}
