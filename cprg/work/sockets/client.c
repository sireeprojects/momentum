#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>
#include <linux/sockios.h>


// You can change the size of the receive and the send buffer as follows (send buffer shown):
// 
// int buffersize = 64*1024;  // 64k
// setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *) &buffersize, sizeof(buffersize));
// and to get the current size, use:
// 
// getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char &) &buffersize, sizeof(buffersize));
// There is a known bug in Linux that reports the current buffer size as 1/2 the set value. I can't remember if the internal value is 1/2 of what was requested, or whether the report is 1/2 of what it really is.
// 
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Sending data: ");
    // sleep (10);
    bzero(buffer,256);
    // fgets(buffer,255,stdin);
    
    int i=0;
    int count=0;
    socklen_t optlen = sizeof (sockfd);
    int size;
    for (i=0; i<100; i++)
    {
       n = write (sockfd, buffer, i);
       getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &size, &optlen); printf ("SO_SNDBUF = %d\n", size);
       sleep (1);
       // ioctl(sockfd, SIOCOUTQ, &count); printf ("SIOCOUTQ = %d\n", count);
    }

    // ioctl(sockfd, TIOCOUTQ, &count);
    
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // bzero(buffer,256);

    // n = read(sockfd,buffer,4);
    // if (n < 0) error("ERROR reading from socket");

    // unsigned int flen=64;
    // memcpy (&flen, buffer, 4);

    // printf("%d\n",flen);
    // // printf("%s\n",buffer);

    close(sockfd);
    return 0;
}
