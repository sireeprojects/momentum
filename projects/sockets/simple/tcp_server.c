#include <stdio.h>
#include <stdint.h>         // uint*
#include <errno.h>          // errno
#include <string.h>         // bzero
#include <stdlib.h>         // exit
#include "msg.h"            // INFO WARNING ERROR DEBUG
#include <sys/socket.h>     // socket
#include <netinet/in.h>     // sockaddr_in
#include <sched.h>          // FD_ macros


#define PORT 8888


int main() {

    int sockfd;
    struct sockaddr_in servaddr;
    fd_set events;
    int max_events;

    // defaults
    FD_ZERO(&events);
    max_events = -1;
    sockfd = -1;

    // create a socket fd
    sockfd = socket (PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERROR("Socket Creation Failed");
        exit(0);
    }
    // prepare address
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 

    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        ERROR("socket bind failed..."); 
        exit(0); 
    } else {
        INFO("Socket successfully binded.."); 
    }
    // listen
    if ((listen(sockfd, 5)) != 0) { 
        ERROR("Listen failed..."); 
        exit(0); 
    } else {
        INFO("Server listening.."); 
    }

    // to monitor sockfd
    FD_SET(sockfd, &events);
    if (sockfd > max_events) {
        max_events = sockfd;
    }
    int fd;
    int ret;

    // event loop
    while (1) {
        uint32_t timeout = 200000;
        struct timeval tv;
        tv.tv_sec = timeout/1000000;
        tv.tv_usec = timeout%1000000;
        fd_set tmp_events = events;

        // check for events
        ret = select(max_events+1, &tmp_events, 0, 0, &tv);

        if (ret == -1) {
            ERROR("Select: %s", strerror(errno));
            exit(0);
        }
        if (ret == 0) {
            continue;
        }
        for (fd=0; fd<max_events+1; fd++) {
            if (FD_ISSET(fd, &tmp_events) && FD_ISSET(fd, &tmp_events)) {
                if (fd == sockfd) {
                    struct sockaddr_in in;
                    socklen_t len = sizeof(in);
                    int client_fd = accept(sockfd, (struct sockaddr*) &in, &len);
                    INFO("Connection request accepted...");
                } else {
                    // process data
                    char instream[32];
                    char outstream[32];
                    int nbytes = recv(fd, (char*)&instream, 32, 0);
                    INFO("Data Received: %s", instream);
                    strcpy(outstream, "message from linux");
                    send(fd, outstream, 32, 0);
                }
            }
        }

    }
    return 0;
}
