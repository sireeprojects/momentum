#include <iostream>
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

using namespace std;

int recv_fd (int sock);
int send_fd (int sock, int fd);

fd_set fds;
unsigned int max_fd;
void add_fd (int fd);




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

