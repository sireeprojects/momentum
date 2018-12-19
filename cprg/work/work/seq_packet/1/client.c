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
   struct sockaddr_un server;
   int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, "/tmp/seqpacket.sock");

   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
       perror("Connecting stream socket");
       close(client_fd);
       exit(1);
   }

   sleep (5);

   printf ("sending some data...\n");

   // dummy read
   // char buf [256];
   // int nbytes;
   // nbytes = recv (client_fd, buf, sizeof buf, 0);

   char buf [256];
   bzero (buf,256);
   int nbytes;
   nbytes = send (client_fd, buf, 10, 0);

   // char buf [256];
   // int nbytes;
   nbytes = recv (client_fd, buf, 10, 0);
   printf ("Write event\n");

   sleep (100);
}
