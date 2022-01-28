#include "winsock2.h"
// #include "windows.h"
#include "tcplib.h"

int main ()
{

// Initialize Winsock.
// WSADATA wsaData;
// int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
// if ( iResult != NO_ERROR )
// printf("Error at WSAStartup()\n");

   printf ("Size of Frame structure  = %d\n", sizeof (struct enet_frame));
   printf ("Size of pseudo structure = %d\n", sizeof (struct pseudo_ip_hdr));

   struct enet_frame *frame;
   frame = (struct enet_frame*) malloc (84);

   /* clear fields */
   memset (frame, 0, 84);

   /* pack pkt1 into frame */
   memcpy (frame, pkt1, 84);

   /* display bytes of frame */
   print_frame_bytes ((unsigned char*) frame, sizeof (struct enet_frame));

   /* pack the frame in network byte order */
   pack_frame_netw (frame);

   /* display the frame structure */
   print_frame (frame);

   return 0;
}
