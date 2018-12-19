#include "common.h"

#define SOCKNAME "/tmp/vubr.sock"
using namespace std;

void respond (control_msg *msg, int fd);

int main () 
{
   // initialize
   int fdmax=-1;
   int loop_cntr = 0;
   unsigned int timeout = 200000;
   fd_set master_set;
   fd_set tmp_set;
   FD_ZERO (&master_set);

   // socket vars
   int server_fd; 
   struct sockaddr_un server;
   socklen_t sock_addr_len;
   int client_fd;

   // connection vars
   int nbytes;
   char buf[256];

   // create socket
   if ((server_fd = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      SMSG ("Socket Call failed"); 
      exit(1);
   }
   SMSG ("Socket Creation Successful");

   // bind to address (in this case local path)
   sock_addr_len = sizeof(struct sockaddr_un);
   server.sun_family = AF_UNIX;
   strncpy (server.sun_path, SOCKNAME, (sock_addr_len - sizeof(short)));
   unlink (server.sun_path);
   if ((bind (server_fd, (struct sockaddr*) &server, sock_addr_len)) == -1)
   {
      SMSG ("Bind Call failed"); 
      exit (1);
   }         
   SMSG ("Bind Successful");
   
   // lister to the open server
   listen (server_fd, 100);
   FD_SET (server_fd, &master_set);
   fdmax = server_fd;

   while (1)
   {
      struct timeval tv; 
      tv.tv_sec = timeout / 1000000;
      tv.tv_usec = timeout % 1000000;

      FD_ZERO (&tmp_set);
      tmp_set = master_set;

      if (select (fdmax+1, &tmp_set, NULL, NULL, &tv) == -1)
      {
         perror ("Select Error");
         exit(1);
      }
     
      int fd;
      for (fd=0; fd<=fdmax; fd++)
      {
         if (FD_ISSET(fd, &tmp_set) && FD_ISSET(fd, &master_set))
         {
            if (fd == server_fd)
            {
               SMSG ("Waiting for new connection");
               struct sockaddr_un un;
               socklen_t len = sizeof(un);
               client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
               SMSG ("New connection request accepted (%d)", client_fd);
               FD_SET (client_fd, &master_set);
               if (client_fd > fdmax) fdmax = client_fd;
            }
            else
            {
               // check if client data is available 
               if ((nbytes = recv (fd, buf, sizeof buf, 0)) <= 0) 
               {
                  if (nbytes == 0) {
                     SMSG ("Cilent Connection Closed");
                  } else {
                     perror("Recv :: ");
                  }
                  close (fd);
                  FD_CLR (fd, &master_set);
               }
               else {
                  // read and print client data
                  control_msg cm;
                  memcpy (&cm, buf, 12);
                  respond (&cm, fd);
               }  
            }
         }
      }
      loop_cntr++;
   }
   return 0;
}

void respond (control_msg *msg, int fd)
{
   control_msg *cm = msg;
   SMSG("Message type = %d", cm->type);
   SMSG("Message res = %lx", cm->reserved);

   switch (cm->type)
   {
      case 5:{
         SMSG ("Responding to data request");
         char b[256] = "Hello from Client !!!\0";
         send (fd, (char*)b, sizeof b, 0);
         break;
         }
      default:{
         SMSG ("Unknown type received");
         break;
         }
       
   }
}
