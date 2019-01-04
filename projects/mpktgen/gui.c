#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "colors.h"

#define CONTROL_PLANE "/tmp/mpktgen_cp.sock"

int main () {
    struct sockaddr_un addr;
    int fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CONTROL_PLANE);
    
    if (connect(fd, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) ){
        printf("Error Connecting stream socket");
        exit(1);
    }
    char cmdIn;
    char cmdStart[8] = "start";
    char cmdStop[8] = "stop";

    while (1) {
        printf("Enter Command:");
        scanf (" %c", &cmdIn);
        if (cmdIn=='p') {
            printf(FGRN("Command:Start\n"));
            send (fd, &cmdStart, 8,  0);
        } else if (cmdIn=='s') {
            printf(FGRN("Command:Stop\n"));
            send (fd, &cmdStop, 8,  0);
        } else if (cmdIn=='e') {
            printf(FGRN("Exit\n"));
            return(0);
        } else {
            printf(FRED("Invalid Command\n"));
        }
    }
    return 0;
}
