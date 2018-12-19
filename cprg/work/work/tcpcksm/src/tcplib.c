#include "tcplib.h"


#define BYTES_PER_LOOP 25


void print_array (uint8_t *array, uint32_t size)
{
   uint32_t i, j, cntr=0;
   uint32_t bytes_per_loop = BYTES_PER_LOOP;
   uint32_t loop_size = size/25;
   uint32_t last_loop = size%25;

   printf ("Size           = %d\n", size);
   printf ("loop size      = %d\n", loop_size);
   printf ("last reminder  = %d\n", last_loop);

   for (i=0; i<loop_size; i=i+1)
   {
      for (j=0; j<bytes_per_loop; j=j+1)
      {
         printf ("%02x ", array[cntr]);
         cntr=cntr+1;
      }
      printf ("\n");
   }
   for (i=0; i<last_loop; i=i+1)
   {
      printf ("%02x ", array[cntr]);
      cntr=cntr+1;
   }
   printf ("\n");
}


void print_frame (struct enet_frame *frame)
{
   printf ("\n");
}
