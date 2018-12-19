#include <stdio.h>
#include "tcplib.h"

int main ()
{
   printf ("Size of Frame structure  = %d\n", sizeof (struct enet_frame));
   printf ("Size of pseudo structure = %d\n", sizeof (struct pseudo_ip_hdr));
   struct enet_frame frame = {};
   struct pseudo_ip_hdr phdr = {};

   frame.mac.dest = 0x112233445566;
   frame.mac.src  = 0xaabbccddeeff;
   frame.tcp.urg_ptr = 0xaabb;

   print_array ((unsigned char*)  &frame, sizeof (struct enet_frame));

   printf ("dest normal = %llx\n" , frame.mac.dest);
   printf ("dest htonl  = %llx\n" , htonl(frame.mac.dest));

   printf ("src normal = %llx\n" , frame.mac.src);
   printf ("src htonl  = %llx\n" , htonl(frame.mac.src));

   print_frame (&frame);

   return 0;
}
