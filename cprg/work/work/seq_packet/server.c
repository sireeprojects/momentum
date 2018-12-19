#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/un.h>


int main () 
{
   int server_fd;
   int client_fd;
   socklen_t len;
   struct sockaddr_un server_addr;
   unsigned int timeout = 200000;
   char buf [6000];
   int nbytes;

   // create socket
   server_fd = socket (AF_UNIX, SOCK_SEQPACKET, 0);
   if (server_fd==-1) perror ("socket failed");

   
   // bind to local address
   len = sizeof(struct sockaddr_un);
   server_addr.sun_family = AF_UNIX;
   strncpy (server_addr.sun_path, "/tmp/seqpacket.sock", (len - sizeof(short)));
   unlink (server_addr.sun_path);
   int success = bind (server_fd, (struct sockaddr*) &server_addr, len);
   if (success==-1) perror ("bind failed");


   // start listening
   listen (server_fd, 1);
   printf ("Waiting for connection request...\n");


   // accept one client connection
   struct sockaddr_un un;
   client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
   printf ("Connection request received from client\n");


   while (1)
   {
      printf ("Waiting for data...\n");
      nbytes = recv (client_fd, buf, 6000, 0);
      printf ("number of bytes received = %d\n", nbytes);
   }
   return 0;
}
