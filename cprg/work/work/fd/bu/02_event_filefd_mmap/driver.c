#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

using namespace std;


fd_set fds;
unsigned int max_fd;
void add_fd (int fd);
int recv_fd (int sock);
int send_fd (int sock, int fd);


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
      }//for

      for (fd=0; fd<m+1; fd++)
      {
         if (fd==server_fd) 
         {
            if (FD_ISSET(fd, &fds_rd))
            {
               struct sockaddr_un un;
               socklen_t len = sizeof(un);
               printf ("Connection request received from client\n");
               client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
               add_fd (client_fd);
            }
         }
         else if (fd==client_fd) 
         {
            if (FD_ISSET(fd, &fds_rd))
            {
               printf ("fd==client_fd\n");
               new_fd = recv_fd (client_fd);
               printf("fd is received = %d\n", new_fd);
               add_fd (new_fd);
               ptr = (char*) mmap (0, 8, PROT_READ | PROT_WRITE, MAP_SHARED, new_fd, 0);
            }
         }
         else if (fd==new_fd) 
         {
            if (FD_ISSET(fd, &fds_rd))
            {
               for (int i=0; i<8; i++)
               {
                  printf ("%c\n", *ptr);
                  ptr = ptr+1;
               }
            }
         }
      }//for
      sleep (1);
      cnt++;
   }//while


   return 0;
} //main


void add_fd (int fd)
{
   FD_SET(fd, &fds);
   if (fd > max_fd) max_fd = fd;
   printf ("Socket Added (sock_id=%d max=%d)\n", fd, max_fd);
}


int send_fd (int sock, int fd)
{
    struct msghdr msg;
    struct iovec iov[1];
    struct cmsghdr *cmsg = NULL;
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];

    memset(&msg, 0, sizeof(struct msghdr));
    memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

    data[0] = ' ';
    iov[0].iov_base = data;
    iov[0].iov_len = sizeof(data);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_controllen =  CMSG_SPACE(sizeof(int));
    msg.msg_control = ctrl_buf;

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *((int *) CMSG_DATA(cmsg)) = fd;

    return sendmsg(sock, &msg, 0);
}

int recv_fd (int sock)
{
    struct msghdr msg;
    struct iovec iov[1];
    struct cmsghdr *cmsg = NULL;
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    char data[1];

    memset(&msg, 0, sizeof(struct msghdr));
    memset(ctrl_buf, 0, CMSG_SPACE(sizeof(int)));

    iov[0].iov_base = data;
    iov[0].iov_len = sizeof(data);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = ctrl_buf;
    msg.msg_controllen = CMSG_SPACE(sizeof(int));
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    recvmsg(sock, &msg, 0);

    cmsg = CMSG_FIRSTHDR(&msg);

    return *((int *) CMSG_DATA(cmsg));
}

//   while (1)
//   {
//      struct timeval tv;
//      tv.tv_sec = timeout / 1000000;
//      tv.tv_usec = timeout % 1000000;
//      fd_set fds_rd = fds;
//      fd_set fds_wr = fds;
//      printf ("Selecting...\n");
//      int rc = select(max_fd+1, &fds_rd, &fds_wr, 0, &tv);
//
//      int fd;
//      int m = max_fd;
//      for (fd=0; fd<m+1; fd++)
//      {
//         printf ("checking for = %d\n", fd);
//
//         if (fd==server_fd) {
//            if (FD_ISSET(fd, &fds_rd))
//            {
//            struct sockaddr_un un;
//            socklen_t len = sizeof(un);
//            printf ("Connection request received from client\n");
//            client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
//            add_fd (client_fd);
//               // char buf [256];
//               // int nbytes;
//               // nbytes = recv (client_fd, buf, sizeof buf, 0);
//            }
//            }
//         else if (fd==client_fd) {
//            if (FD_ISSET(fd, &fds_wr))
//            {
//            printf ("fd==client_fd\n");
//            new_fd = recv_fd (client_fd);
//            printf("fd is received = %d\n", new_fd);
//            add_fd (new_fd);
//            }
//            break;
//            }
//         else if (fd==new_fd) {
//            printf ("fd==new_fd\n");
//            char buf [256];
//            int nbytes;
//            nbytes = read (new_fd, buf, 100);
//            printf("fd is updated = %d\n", nbytes);
//            break;
//            }
//      }//for
//   }//while
