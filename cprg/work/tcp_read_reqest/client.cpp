#include "common.h"

#define SOCKNAME "/tmp/vubr.sock"

int main ()
{
   struct sockaddr_in serv_addr;
   struct hostent *server;
   // create socket
   int portno = 8888;
   int client_fd = socket(AF_INET, SOCK_STREAM, 0);

   server = gethostbyname("127.0.0.1");
   if (server == NULL) {
       fprintf(stderr,"ERROR, no such host\n");
       exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);

   // request a connection from server
   if (connect(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
       close(client_fd);
       CMSG("Socket Error");
       exit(1);
   }

   // wait for sometime before any operation
   sleep(5);

   // tx_buf = new unsigned char [MAX_FRAME_SIZE];
   char buf[12];
   bzero(&buf, 12);
   buf[11] = 0 ;
   strcpy (buf, "abcd");
   

   // int num_bytes = read (client_fd, buf, 10240);
   CMSG("Sending data..");
   send (client_fd, buf, 12, 0);

   sleep(100);

   // request rng information
   // cm.type = 5;
   // cm.reserved = 0xaabbccdd11223344;
   // send (client_fd, (char*)&cm, 12, 0);

   // read response from adaptor and print ring information
   // int nbytes = recv (client_fd, (char*)&ri, sizeof ri, 0);

   return 0;
}
