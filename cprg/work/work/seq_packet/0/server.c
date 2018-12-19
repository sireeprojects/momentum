#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/un.h>


fd_set fds;
unsigned int max_fd;
void add_fd (int fd);


int main () 
{
   int server_fd;
   int client_fd;
   socklen_t len;
   struct sockaddr_un server_addr;
   unsigned int timeout = 200000;
   

   // create socket
   server_fd = socket (AF_UNIX, SOCK_SEQPACKET, 0);
   if (server_fd==-1) perror ("socket failed");
   add_fd (server_fd);

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


   while (1)
   {
      struct timeval tv;
      tv.tv_sec = timeout / 1000000;
      tv.tv_usec = timeout % 1000000;
      fd_set fds_rd = fds;
      fd_set fds_wr = fds;
      int rc = select(max_fd+1, &fds_rd, &fds_wr, 0, &tv);

      int fd;
      for (fd=0; fd<max_fd+1; fd++)
      {
         if (fd == server_fd) //process server events
         {
            if (FD_ISSET(fd, &fds_rd))
            {
                  struct sockaddr_un un;
                  socklen_t len = sizeof(un);
                  printf ("Connection request received from client\n");
                  client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
                  add_fd (client_fd);
               char buf [256];
               int nbytes;
               nbytes = recv (client_fd, buf, sizeof buf, 0);
            }
         }
         else if (fd==client_fd)
         {
            if (FD_ISSET(fd, &fds_rd))
            {
               printf ("Read event\n");
            }
            else if (FD_ISSET(fd, &fds_wr))
            {
               printf ("Write event\n");
               char buf [256];
               int nbytes;
               nbytes = recv (client_fd, buf, sizeof buf, 0);
            }
         }
      }
   } //while


   return 0;
} //main


void add_fd (int fd)
{
   FD_SET(fd, &fds);
   if (fd > max_fd) max_fd = fd;
   printf ("Socket Added (sock_id=%d max=%d)\n", fd, max_fd);
}



               // char buf [256];
               // int nbytes;
               // nbytes = recv (fd, buf, sizeof buf, 0);
               //
      // int fd;
      // for (fd=0; fd<max_fd+1; fd++)
      // {
      //    if (FD_ISSET(fd, &fds_rd) && FD_ISSET(fd, &fds))
      //    {
      //       if (fd == server_fd) //connection request
      //       {
      //          struct sockaddr_un un;
      //          socklen_t len = sizeof(un);
      //          printf ("Connection request received from client\n");
      //          int client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
      //          add_fd (client_fd);
      //       }
      //       else //event on client fd
      //       {
      //          printf ("some activity...\n");
      //          // dummy write 
      //          char buf [256];
      //          int nbytes;
      //          nbytes = send (fd, buf, sizeof buf, 0);
      //       }
      //    }
      // }
