// compile with: gcc -static -o sendfd sendfd.c
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <strings.h>

int send_fd(int sock, int fd){
    // This function does the arcane magic for sending
    // file descriptors over unix domain sockets
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


int main(int argc, char **argv){
    struct sockaddr_un addr;
    int sock;
    int conn;
    int fd;

    // Create a unix domain socket
    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    // Bind it to a abstract address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(&addr.sun_path[1], argv[1]);
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // Open the file descriptor we want to send
    fd = open(argv[2], 0);

    // Listen
    listen(sock, 1);

    // Just send the file descriptor to anyone who connects
    for(;;){
        conn = accept(sock, NULL, 0);
        send_fd(conn, fd);
        close(conn);
    }

    return 0;
}
