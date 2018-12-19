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
   int client_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, "/tmp/seqpacket.sock");

   int success = connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un));

   if (success<0)
   {
       printf("Error Connecting stream socket");
       exit(1);
   }

   sleep (5);

   printf ("sending some data...\n");

   char buf [5000];
   int nbytes;
   nbytes = send (client_fd, buf, 2500,  0);
   nbytes = send (client_fd, buf, 5000, 0);

   sleep (100);
}
