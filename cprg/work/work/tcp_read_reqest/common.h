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


#define SMSG(...)                \
   printf ("\n[Server] " __VA_ARGS__); \
   fflush(stdout);

#define CMSG(...)                \
   printf ("\n[Client] " __VA_ARGS__); \
   fflush(stdout);

#define DMSG(...)                \
   printf ("\n[Data] " __VA_ARGS__); \
   fflush(stdout);

