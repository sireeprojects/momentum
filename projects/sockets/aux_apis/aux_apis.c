#include <stdio.h>
#include <stdint.h>
#include "msg.h"
#include <netinet/in.h> // htons ntohs htonl ntohl
#include <netdb.h>      // gethostname

int main ()
{
   uint16_t in16, out16;
   uint32_t in32, out32;
   uint32_t size=1000;
   char buf[size];

   TITLE("Testing htons");
   in16 = 0x1234;
   out16 = htons(in16);
   printf("in=0x%x out=0x%x\n", in16, out16);

   TITLE("Testing ntohs");
   in16 = 0x1234;
   out16 = ntohs(in16);
   printf("in=0x%x out=0x%x\n", in16, out16);

   TITLE("Testing htonl");
   in32 = 0xaabbccdd;
   out32 = htonl(in32);
   printf("in=0x%x out=0x%x\n", in32, out32);

   TITLE("Testing ntohl");
   in32 = 0xaabbccdd;
   out32 = ntohl(in32);
   printf("in=0x%x out=0x%x\n", in32, out32);

   TITLE("Testing gethostname");
   gethostname(buf,size);
   printf ("%s\n", buf);

   TITLE("Testing gethostbyname");
   struct hostent *host;
   host = gethostbyname(buf);
   if (host==NULL) WARNING ("gethostbyname failed")
   printf ("%s", host->h_name);
}
