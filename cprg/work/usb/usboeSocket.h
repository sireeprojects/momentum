#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>

class usboeSocket 
{
   public:

      usboeSocket();

      // Server APIs
      void init_server(int port);
      void wait_for_client();
      void start_server();
      int wait_for_connection_req(unsigned int timeout);
      int read_data (char *buf);
      void send_data (char *buf, int len);
      void close_server();

      // client API 
      void connect_to_server(char *server_name, int port);
      void close_client();

   private:

      // Server vars
      int server_fd;
      struct sockaddr_in server_addr; 
      std::thread th_wait_for_conn;
      fd_set fdset;      
      int fd_max;
      struct hostent *server;

      // Client vars
      int client_fd;
      socklen_t cli_len;
      struct sockaddr_in client_addr; 

      // misc
      void error(const char *msg);
};
