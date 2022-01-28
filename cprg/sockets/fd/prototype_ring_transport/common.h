#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <iomanip>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>


#define RING_SIZE 10
#define PAGE_SIZE 100
#define VRING_DESC_F_WRITE 2
#define VRING_DESC_F_NEXT  1


using namespace std;


string readable_fs (double size)
{
    int i = 0;
    char buf[100];
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1023) {
      size /= 1024;
      i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}


struct vring_desc 
{
   uint64_t addr;
   uint32_t len;
   uint16_t flags;
   uint16_t next;
};


struct vring_used_elem 
{
   uint32_t id;
   uint32_t len;
};


struct vring_used
{
   uint16_t flags;
   uint16_t idx;
   struct vring_used_elem ring[RING_SIZE];
};


struct vring_avail
{
   uint16_t flags;
   uint16_t idx;
   uint16_t ring[RING_SIZE];
};


struct page 
{
   char mem [PAGE_SIZE];
};


struct virtqueue
{
   struct vring_desc desc [RING_SIZE];
   struct vring_avail avail;
   struct vring_used used;
   struct page guest_mem[RING_SIZE];
};


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
