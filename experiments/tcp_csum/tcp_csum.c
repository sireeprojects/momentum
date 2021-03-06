#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// // Frame 84 bytes
// uint8_t pkt[84]={
//    0x08, 0x00, 0x27, 0x38, 0xdb, 0xed, 0x08, 0x00,
//    0x27, 0x97, 0x3f, 0x45, 0x08, 0x00, 0x45, 0x00,
//    0x00, 0x46, 0x00, 0xec, 0x40, 0x00, 0x80, 0x06,
//    0xf5, 0xc1, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01,
//    0x01, 0x01, 
//                0x04, 0x12, 0x00, 0x35, 0xd1, 0xf8, // tcp header start
//    0xc1, 0x17, 0x5f, 0xf5, 0xa8, 0xbd, 0x50, 0x18,
//    0xfb, 0x90, 0x00, 0x00, 0x00, 0x00,             // checksum 05 68 (3,4)
//                                        0x00, 0x1c, // payload data
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x04, 0x65, 0x74, 0x61,
//    0x73, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0xfc,
//    0x00, 0x01, 0x4d, 0x53                         
// };

uint16_t data16[31]={
   0x0101, 
   0x0102,
   0x0101, 
   0x0101,
   0x0006,                  
   0x0032,
   0x0412, 
   0x0035, 
   0xd1f8,
   0xc117, 
   0x5ff5, 
   0xa8bd, 
   0x5018,
   0xfb90, 
   0x0000, 
   0x0000,           
   0x001c,
   0x0000, 
   0x0000, 
   0x0001, 
   0x0000,
   0x0000, 
   0x0000, 
   0x0465, 
   0x7461,
   0x7303, 
   0x636f, 
   0x6d00, 
   0x00fc,
   0x0001, 
   0x4d53                         
};

int main()
{
   uint16_t csum;
   uint8_t *data;
   uint16_t *data16;
   data = (uint8_t*) malloc (62);
   data16 = (uint16_t*) malloc (31);

   memcpy (data, pseudo_hdr, 12);
   memcpy (data+12, (pkt+34), 50);

   memcpy (data16, (uint16_t*) data, 62);

   /* print packet bytes */
   uint16_t i;
   printf ("Packet Bytes:\n");

   for(i=0; i<62; i=i+1) {
      printf ("%02x ", data[i]);
      if (i%8==7) 
         printf (" ");
      if (i%16==15) 
         printf ("\n");
   }

   printf ("\n");

   printf ("Packet Bytes:\n");

   for(i=0; i<31; i=i+1) {
      printf ("%04x ", data16[i]);
      if (i%8==7) 
         printf (" ");
      if (i%16==15) 
         printf ("\n");
   }

   printf ("\n");

   csum = compute_tcp_checksum(data16, 62);
   printf ("Computer Checksum = %04x \n", csum); // expected 05 68

   return 0;
}
