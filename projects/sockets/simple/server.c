#include <stdio.h>
#include "msg.h"
#include <sys/socket.h>

int main () {
   // create a socket
   int sockfd;
   sockfd = socket (PF_UNIX, SOCK_STREAM, 0);
   if (sockfd<0) ERROR("Socket Creation Failed");

   // prepare a local address structure

   // Bind the socket to a local address (above)
   // on which the server listens for incoming 
   // connection requests
 
   // start listening on the local address


   // when there is a new connection request
   // accept the request and make a connection with 
   // the incoming address

   // exchange data

}
