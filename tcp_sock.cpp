#include <iostream>
#include <sys/select.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <assert.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <netdb.h> 
#include <pthread.h>
#include <thread>

using namespace std;

// 0:non-blocking, 1:blocking
int fd_set_blocking(int fd, int blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return 0;
    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags) != -1;
}

typedef enum {
    START,
    STOP,
    CLEAR_STATS
} command;

typedef enum {
    PORT_INFO,
    PORT_STATS,
    COMMAND
} metadata_type;

struct __attribute__((packed)) port_info {
    char port_name[32];
    uint32_t port_num : 32;
    char pad[24];
};

struct __attribute__((packed)) port_stats {
    uint32_t port_num : 32;
    uint64_t transmitted : 64;
    uint64_t received : 64;
    char pad[40];
};

struct __attribute__((packed)) port_cmds {
    uint32_t port_num : 32;
    uint32_t cmd : 32;
    char pad[52];
};

struct __attribute__((packed)) metadata {
    metadata_type type : 32;
    union {
        port_info info;
        port_stats stats;
        port_cmds cmd;
    };
};

bool port_done = false;

// initialize
int fdmax=-1;
uint32_t timeout = 200000;
fd_set master_set;
fd_set readset;
fd_set writeset;

// socket vars
int server_fd; 
struct sockaddr_in server;
int client_fd;
int portno;

// connection vars
int nbytes;
char buf[64];
pthread_t worker;
void *sock_server(void*);

bool done;
pthread_t stats;
void *stats_server(void*);

int main () 
{
    FD_ZERO (&master_set);

   // create socket
   if ((server_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
      printf ("Socket Call failed\n"); 
      exit(1);
   }
   printf ("Socket Creation Successful\n");

   // bind to address (in this case local path)
   bzero((char *) &server, sizeof(server));
   portno = 8889;
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(portno);

   if ((bind (server_fd, (struct sockaddr*) &server, sizeof(server))) == -1) {
      printf ("Bind Call failed\n"); 
      exit (1);
   }         
   printf ("Bind Successful\n");
   
   // lister to the open server
   listen (server_fd, 100);
   FD_SET (server_fd, &master_set);
   fdmax = server_fd;

   pthread_create(&worker, NULL, &sock_server, NULL);

   pthread_join(worker, NULL);
   return 0;
}

void *sock_server(void*) {
   while (1) {
      struct timeval tv; 
      tv.tv_sec = timeout / 1000000;
      tv.tv_usec = timeout % 1000000;

      FD_ZERO (&readset);
      readset = master_set;
      writeset = master_set;

      if (select (fdmax+1, &readset, &writeset, NULL, &tv) == -1) {
         perror ("Select Error");
         exit(1);
      }
     
      int fd;
      for (fd=0; fd<=fdmax; fd++) {

         if (fd == server_fd && FD_ISSET(fd, &readset)) {
            printf ("Waiting for new connection\n");
            struct sockaddr_in un;
            socklen_t len = sizeof(un);
            client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
            printf ("New connection request accepted (%d)\n", client_fd);
            FD_SET (client_fd, &master_set);
            if (client_fd > fdmax) fdmax = client_fd;
            port_done = false;
         }

         if (fd == client_fd && FD_ISSET(fd, &readset)) {
            bzero(&buf, 64);
            if ((nbytes = recv (fd, buf, sizeof buf, 0)) <= 0) {
               if (nbytes == 0) {
                  printf ("Cilent Connection Closed\n");
                  done = true;
               } else {
                  perror("Recv ");
               }
               close (fd);
               FD_CLR (fd, &master_set);
            } else {
                metadata meta;
                memcpy((char*)&meta, buf, 64);
                switch (meta.type) {
                    case COMMAND: {
                        switch (meta.cmd.cmd) {
                            case START: {
                                printf("Received command: Start\n");
                                done = false;
                                pthread_create(&stats, NULL, &stats_server, NULL);
                                break;
                                }
                            case STOP: {
                                printf("Received command: Stop\n");
                                done = true;
                                break;
                                }
                            case CLEAR_STATS: {
                                break;
                                }
                        } //switch
                        break;
                        } // command
                } //switch
            }
         }
      }
   }
}


void *stats_server(void*) {
    metadata meta;

    if (!port_done) {
        meta.type = PORT_INFO;
        for(int i=0; i<8; i++) {
            meta.info.port_num = i;
            sprintf(meta.info.port_name, "Port_%d", i);
            write(client_fd, (char*)&meta, 64);
        }
        port_done = true;
    }

    int txcnt = 0;

    meta.type = PORT_STATS;
    while(!done) {
        for(int i=0; i<8; i++) {
            meta.stats.port_num = i;
            meta.stats.transmitted = txcnt;
            write(client_fd, (char*)&meta, 64);
        }
        txcnt += 100;
        this_thread::sleep_for(std::chrono::milliseconds(100));
    };
    printf("Transmit thread stopped\n");
}
