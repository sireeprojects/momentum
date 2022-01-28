#include "common.h"

#define SOCKNAME "/tmp/vubr.sock"

int main ()
{
   struct sockaddr_un server;
   // create socket
   int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, SOCKNAME);
   
   // request a connection from server
   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
       close(client_fd);
       CMSG("Socket Error");
       exit(1);
   }

   // wait for sometime before any operation
   sleep(5);

   // tx_buf = new unsigned char [MAX_FRAME_SIZE];
   char buf[256];
   bzero(&buf, 256);

   CMSG("Before read");
   int num_bytes = read (client_fd, buf, 10240);
   CMSG("After read");

   // request rng information
   // cm.type = 5;
   // cm.reserved = 0xaabbccdd11223344;
   // send (client_fd, (char*)&cm, 12, 0);

   // read response from adaptor and print ring information
   // int nbytes = recv (client_fd, (char*)&ri, sizeof ri, 0);

   return 0;
}
