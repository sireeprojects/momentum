#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#define REMOTE_HOST "192.168.0.100"
#define REMOTE_PORT 9999

int main(int argc, char *argv[]) {
    int sockfd = 0, n = 0;
    char buf[32];
    struct sockaddr_in server; 

    memset(buf, '0',sizeof(buf));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 
    memset(&server, '0', sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_port = htons(REMOTE_PORT); 

    if(inet_pton(AF_INET, REMOTE_HOST, &server.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
    if( connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    } 
    unsigned long i;
    int j;
    char recvBuf[10];

    for (j=0; ;j++) {
        for (i=0; i<1000000; i++) {}
        // usleep(1000);
        memcpy(recvBuf, (char*)&j, 4);
        memcpy(recvBuf+4, (char*)&i, 8);
        write(sockfd, recvBuf, 12);
    }
    sleep(5);
    close(sockfd);
    return 0;
}
