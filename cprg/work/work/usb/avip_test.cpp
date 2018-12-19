#include <iostream>
#include <libusb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "usboeSocket.h"

using namespace std;

int main()
{
   usboeSocket sock;
   sock.init_server(8888);
   sock.wait_for_client();
   char data[1024];
   unsigned int DL = 8;

   while (1)
   {
      int len = sock.read_data (data);

      printf ("Data Received from Remote USB Client -> ");
      for (int i=0 ;i < DL; i++) 
         printf("%02x;",data[i]);
      printf("\n");

      if (data[2]==0x29)
         break;
   }
   printf ("Closing Socket connection\n");
   sock.close_server ();
   return 0;
}
