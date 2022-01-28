#include "usboeSocket.h"

void usboeSocket::init_server(int port)
{
   printf (">> Initializing UsbOE Server at Port: %d\n", port);

   // create server endpoint
   server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd < 0)
      error("ERROR Opening Socket");

   // reset server address
   bzero((char *) &server_addr, sizeof(server_addr));

   // prepare server address
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(port);

   // bind server endpoint to server address
   if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
      error("ERROR On Binding");

   // start listening for connections, accept only 2 connections
   listen(server_fd, 2);

   // add server fd for polling
   // FD_SET (server_fd, &fdset);
   // fd_max = server_fd;
   // printf (">> Adding Server FD=%d for polling\n", server_fd);
   // start thread to listen for connection requests
   // th_wait_for_conn = std::thread (&usboeSocket::start_server, this);
}

void usboeSocket::wait_for_client()
{
   cli_len = sizeof (client_addr);
   printf (">> Waiting for Remote USB Client to request connection...\n");
   client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &cli_len);
   if (client_fd < 0)
      error("ERROR On Accept");
   printf (">> Remote USB Client is now connected to server\n");
}

void usboeSocket::start_server()
{
   printf (">> Starting UsbOE Server\n");
   while (1)
   {
      unsigned int timeout = 200000;
      wait_for_connection_req (timeout);
   }
}

void usboeSocket::close_server ()
{
   close (server_fd);
}

void usboeSocket::close_client ()
{
   close (client_fd);
}

int usboeSocket::read_data (char *buf)
{
   return read (client_fd, buf, 10240);
}

void usboeSocket::send_data (char *buf, int len)
{
   // send (client_fd, buf, 1024);
   write (client_fd, buf, len);
}


void usboeSocket::connect_to_server (char *server_name, int port)
{
   printf (">> Trying to connect to AVIP Server\n");
   client_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (client_fd < 0)
      error("ERROR Opening Socket");
   server = gethostbyname(server_name);

   if (server == NULL) 
   {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &client_addr, sizeof(client_addr));

   client_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&client_addr.sin_addr.s_addr, server->h_length);
   client_addr.sin_port = htons(port);

   if (connect(client_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0)
      error("ERROR Connecting Client Socket");
   printf (">> Connected to AVIP Server!\n");
}

int usboeSocket::wait_for_connection_req(unsigned int timeout)
{
   struct timeval tv;
   tv.tv_sec = timeout / 1000000;
   tv.tv_usec = timeout % 1000000;

   // context switch
   fd_set fdset_tmp = fdset;

   // poll changes in registered fds till timeout
   int rc = select (fd_max+1, &fdset_tmp, 0, 0, &tv);

   // error in select
   if (rc == -1) perror ("Select");

   // timeout, happens often hence no print statements
   if (rc == 0) return 0;
   
   // accept the client connection
   int sock;
   for (sock=0; sock<fd_max+1; sock++)
   {
      if (FD_ISSET(sock, &fdset_tmp))
      {
         if (sock == server_fd)
         {
            //TODO
         }
      }
   }
}

usboeSocket::usboeSocket()
{
   FD_ZERO (&fdset);
   fd_max = -1;
}

void usboeSocket::error(const char *msg)
{
   perror(msg);
   exit(1);
}
