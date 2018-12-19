// compile with gcc -static -o recvfd recvfd.c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>

static int recv_fd(int sock){
    // This function does the arcane magic recving
    // file descriptors over unix domain sockets
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


int main(int argc, char **argv){
    struct sockaddr_un addr;
    int sock;

    // Create and connect a unix domain socket
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family  = AF_UNIX;
    strcpy(&addr.sun_path[1], argv[1]);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    // Recvive a file descriptor, and make it our CWD
    fchdir(recv_fd(sock));

    // Execute a program specified on the command line
    return execvp(argv[2], &argv[2]);
}
