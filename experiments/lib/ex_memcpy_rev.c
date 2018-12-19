#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void memcpy_rev(uint8_t *dest, uint8_t *src, size_t size)
{
   uint16_t i=0;
   for (i=0; i<size; i++) {
      dest[i] = src[size-i-1];
      /* normal memcpy behaviour */
      /* dest[i] = src[i]; */
   }
}

int main ()
{
   uint16_t i;
   uint64_t s = 0x1122334455667788;
   uint8_t *d = (uint8_t*) malloc (8);

   memcpy_rev (d ,(uint8_t*)&s, 8);

   for (i=0; i<8; i++)
      printf ("%02x",d[i]);

   return 0;
}
