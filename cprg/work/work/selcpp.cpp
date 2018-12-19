#include <iostream>
#include <stdio.h>
#include <cstdlib>
// #include <sys/socket.h>
// #include <sys/un.h>
// #include <sys/unistd.h>
#include <sys/eventfd.h>

using namespace std;

int server_fd;
int client_fd;
// socklen_t len;
// struct sockaddr_un server;
// struct sockaddr_un client;

#define SOCKNAME "/tmp/vubr.sock"

class disp {
public:
   disp(){};
   fd_set master;
   int fdmax;

   void set() {
      FD_ZERO(&master);
      FD_SET(client_fd, &master);
      fdmax = client_fd; 
   }
};

int main() {
   int len = sizeof(struct sockaddr_un);

   if ((server_fd = socket (AF_UNIX, SOCK_STREAM, 0)) == -1) {
      perror ("catch - socket failed");
      exit (0);
   }
   server.sun_family = AF_UNIX;
   strncpy (server.sun_path, SOCKNAME, (len - sizeof(short)));
   unlink (server.sun_path);

   if ((bind(server_fd, (struct sockaddr*) &server, len)) == -1) {
      perror ("catch - bind failed");
      exit (0);
   }
   listen (server_fd ,1);
   // client_fd = accept (server_fd,  (struct sockaddr*) &client, &len);
   // printf ("Connection established\n");

   disp d;
   d.set();

   int rc = select (d.fdmax+1, &d.master, NULL, NULL, NULL);
}
