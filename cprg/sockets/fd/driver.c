#include "common.h"


int main () 
{
   int server_fd = -1;
   int client_fd = -1;
   int new_fd =-1;
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

   unsigned int cnt=0;
   unsigned int avail=0;

   bool flag=false;
   char *ptr;
   char *cptr;

   while (1)
   {
      struct timeval tv;
      tv.tv_sec = timeout / 1000000;
      tv.tv_sec = timeout / 1000000;
      tv.tv_usec = 0;
      fd_set fds_rd = fds;
      fd_set fds_wr = fds;
      fd_set fds_ex = fds;
      printf ("Selecting----------------------------%d\n", cnt);
      int rc = select(max_fd+1, &fds_rd, &fds_wr, &fds_ex, &tv);

      int fd;
      int m = max_fd;
      string status;
      for (fd=0; fd<m+1; fd++)
      {
         if (FD_ISSET (fd, &fds_rd))
            status = "RD";
         else if (FD_ISSET (fd, &fds_wr))
            status = "WR";
         else if (FD_ISSET (fd, &fds_ex))
            status = "EX";
         ioctl(fd, FIONREAD, &avail);
         printf ("fd=%d : %s : %d\n", fd, status.c_str(), avail);
      }

      for (fd=0; fd<m+1; fd++)
      {
         if (fd==server_fd && FD_ISSET(fd, &fds_rd)) {
            struct sockaddr_un un;
            socklen_t len = sizeof(un);
            printf ("Connection request received from client\n");
            client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
            add_fd (client_fd);
            }
         else if (fd==client_fd && FD_ISSET(fd, &fds_rd)) {
            printf ("fd==client_fd\n");
            new_fd = recv_fd (client_fd);
            printf("fd is received = %d\n", new_fd);
            add_fd (new_fd);
            ptr = (char*) mmap (0, 8, PROT_READ | PROT_WRITE, MAP_SHARED, new_fd, 0);
            }
         else if (fd==new_fd && FD_ISSET(fd, &fds_rd)) {
            for (int i=0; i<8; i++) {
               printf ("%c\n", *ptr);
               ptr = ptr+1;
               }
            }
      }//for
      sleep (1);
      cnt++;
   }//while


   return 0;
} //main
