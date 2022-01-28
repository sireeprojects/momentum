#include <iostream>
#include <stdlib.h>   
#include <cmath>   

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

using namespace std;

int main () 
{
   unsigned int len;
   unsigned int chunk_len;
   unsigned int chunk_to_write;
   unsigned int remaining_len;
   unsigned int meta_len;

   chunk_len = 4096;
   len=9000;
   meta_len = 48;

   remaining_len = len;
   int nbufs = (ceil) ((float) (len+meta_len)/chunk_len);

   cout << "Nbufs = " << nbufs << endl;

   for (int cnt=0; cnt<nbufs; cnt++)
   {
      if (cnt==0)
      {
         chunk_to_write = meta_len;
         cout  << "INFO_0 ==>" \
               << " Len = " << len \
               << " Chunk to write = " << chunk_to_write \
               << " remaining_len = " << remaining_len << endl;

         chunk_to_write = MIN(remaining_len, chunk_len-meta_len);
         remaining_len = remaining_len - chunk_to_write;
         cout  << "INFO_0 ==>" \
               << " Len = " << len \
               << " Chunk to write = " << chunk_to_write \
               << " remaining_len = " << remaining_len << endl;
      }
      else
      {
         chunk_to_write = MIN(remaining_len, chunk_len);
         remaining_len = remaining_len - chunk_to_write;
         cout  << "INFO_N ==>" \
               << " Len = " << len \
               << " Chunk to write = " << chunk_to_write \
               << " remaining_len = " << remaining_len << endl;
      }
   }

   return 0;
}
