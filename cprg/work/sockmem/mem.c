#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sys/eventfd.h>

using namespace std;
int server_fd;
int client_fd;
socklen_t len;
struct sockaddr_un server;
struct sockaddr_un client;

#define SOCKNAME "/tmp/vubri1.sock"

int main()
{

   int len = sizeof(struct sockaddr_un);
   if ((server_fd = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      perror ("catch - socket failed");
      exit (0);
   }
   server.sun_family = AF_UNIX;
   strncpy (server.sun_path, SOCKNAME, (len - sizeof(short)));
   unlink (server.sun_path);
   if ((bind(server_fd, (struct sockaddr*) &server, len)) == -1)
   {
      perror ("catch - bind failed");
      exit (0);
   }
   listen (server_fd ,1);

   // int sendbuff = 38582912;
   // int res = setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));

   // if(res == -1)
   //   printf("Error setsockopt");

   int n;      
   unsigned int m = sizeof(n);
   getsockopt(server_fd,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);
   cout << "RX Socket size = "<< dec << n << endl; 

   int o;
   unsigned int p = sizeof(o);
   getsockopt(server_fd,SOL_SOCKET,SO_SNDBUF,(void *)&o, &p);
   cout << "TX Socket size = "<< dec << o << endl; 

   sleep(1);

   return 0;
}
