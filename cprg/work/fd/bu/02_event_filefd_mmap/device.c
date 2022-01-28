#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>

int recv_fd (int sock);
int send_fd (int sock, int fd);

int main ()
{
   struct sockaddr_un server;
   int client_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, "/tmp/seqpacket.sock");

   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
      perror("Connecting stream socket");
      close(client_fd);
      exit(1);
   }

   sleep (2);

   // shared memory
   int file_fd = 0;
   char *ptr;
   file_fd = shm_open("shm_test", O_CREAT | O_RDWR, 0777);
   ftruncate(file_fd, 8);

   ptr = (char*) mmap (0, 8, PROT_WRITE, MAP_SHARED, file_fd, 0);
   const  char *message = "12345678";
   memcpy (ptr, message, 8);

   printf ("Sending FD = %d\n", file_fd);
   send_fd (client_fd, file_fd);


   // EventFD
   // int file_fd = eventfd(0, EFD_NONBLOCK);
   // printf ("Sending FD = %d\n", file_fd);
   // send_fd (client_fd, file_fd);
   // sleep (5);
   // eventfd_write(file_fd, 1);
   // sleep (10);
   // eventfd_write(file_fd, 1);


   // // Regular FILE
   // char buf [256];
   // bzero (buf,256);
   // int file_fd = open ("file.txt", O_RDWR);
   // printf ("Sending FD = %d\n", file_fd);
   // send_fd (client_fd, file_fd);

   // sleep (5);
   // write (file_fd, buf, 1);
   // fsync (file_fd);
   // sleep (10);
   // write (file_fd, buf, 1);
   // fsync (file_fd);

   sleep (100);
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
