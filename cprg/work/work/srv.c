#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#define NAME "/tmp/vubr_1.sock"


main()
{
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf[1024];


    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening stream socket");
        exit(1);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, NAME);
    unlink (NAME);
    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
        perror("binding stream socket");
        exit(1);
    }
    printf("Socket has name %s\n", server.sun_path);
    listen(sock, 5);
    for (;;) {
        msgsock = accept(sock, 0, 0);
        printf ("accept success");
        if (msgsock == -1)
            perror("accept");
        else do {
            bzero(buf, sizeof(buf));
            if ((rval = read(msgsock, buf, 1024)) < 0)
                perror("reading stream message");
            else if (rval == 0)
                printf("Ending connection\n");
            else {
                printf("-->%s\n", buf);
                write (msgsock, buf, 64);
            }
        } while (rval > 0);
        close(msgsock);
    }
    close(sock);
    unlink(NAME);
}
