#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define SOCKNAME "/tmp/sock"
#define BUFSIZE 32*1024-1

int main () 
{

   int server_fd;
   int client_fd;
   int len;
   struct sockaddr_un server;
   struct sockaddr_un client;

   len = sizeof(struct sockaddr_un);

   //-----------------------
   // create socket endpoint
   //-----------------------
   if ((server_fd = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      perror ("catch - socket failed");
      return (0);
   }

   //-----------------------
   // prepare socket address
   //-----------------------
   server.sun_family = AF_UNIX;
   strncpy (server.sun_path, SOCKNAME, (len - sizeof(short)));

   //--------------------
   // Bind socket to file
   //--------------------
   unlink (server.sun_path); // remove any previous connection
   if ((bind(server_fd, (struct sockaddr*) &server, len)) == -1) 
   {
      perror ("catch - bind failed");
      return (0);
   }

   //----------------------------------------
   // prepare socket to listen to connections
   //----------------------------------------
   listen (server_fd ,1);

   //--------------------------------------
   // accept connection request from client
   //--------------------------------------
   client_fd = accept (server_fd,  (struct sockaddr*) &client, &len);

   printf ("Connection established\n");

   int recv_len;
   char buf[100];

   while (1)
   {
      recv_len = recv (client_fd, buf, 100,0);
      // if (recv_len !=0 )
      // {
         printf ("Received==> %d = %s\n", recv_len, buf);
      // }
      // else 
         // return (0);
      // receive
      // send 
   }

   return (0);
}

