#include "common.h"

#define SOCKNAME "/tmp/vubr.sock"
using namespace std;

int main () 
{
   // initialize
   int fdmax=-1;
   unsigned int timeout = 200000;
   fd_set master_set;
   fd_set readset;
   fd_set writeset;
   FD_ZERO (&master_set);

   // socket vars
   int server_fd; 
   struct sockaddr_in server;
   int client_fd;
   int portno;

   // connection vars
   int nbytes;
   char buf[256];

   srand(time(NULL));

   // create socket
   if ((server_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
   {
      SMSG ("Socket Call failed"); 
      exit(1);
   }
   SMSG ("Socket Creation Successful");

   // bind to address (in this case local path)
   bzero((char *) &server, sizeof(server));
   portno = 8888;
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(8888);

   if ((bind (server_fd, (struct sockaddr*) &server, sizeof(server))) == -1)
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

      FD_ZERO (&readset);
      readset = master_set;
      writeset = master_set;

      if (select (fdmax+1, &readset, &writeset, NULL, &tv) == -1)
      {
         perror ("Select Error");
         exit(1);
      }
     
      int fd;
      for (fd=0; fd<=fdmax; fd++)
      {
         if (fd == server_fd && FD_ISSET(fd, &readset)) {
            SMSG ("Waiting for new connection");
            struct sockaddr_in un;
            socklen_t len = sizeof(un);
            client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
            SMSG ("New connection request accepted (%d)", client_fd);
            FD_SET (client_fd, &master_set);
            if (client_fd > fdmax) fdmax = client_fd;
            }
         if (fd == client_fd && FD_ISSET(fd, &readset))
         {
            bzero(&buf, 256);
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
               SMSG ("Data available in client socket");
               int num_bytes = read (client_fd, buf, 10240);
               SMSG ("Data = %s", buf);
               // printf ("\nData = %s", buf);
            }
         }
      } // for loop
   }
   return 0;
}

//      for (fd=0; fd<=fdmax; fd++)
//      {
//         if (FD_ISSET(fd, &readset) && FD_ISSET(fd, &master_set))
//         {
//            if (fd == server_fd && FD_ISSET(fd, &readset))
//            {
//               SMSG ("Waiting for new connection");
//               struct sockaddr_un un;
//               socklen_t len = sizeof(un);
//               client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
//               SMSG ("New connection request accepted (%d)", client_fd);
//               FD_SET (client_fd, &master_set);
//               if (client_fd > fdmax) fdmax = client_fd;
//               SMSG ("New FdMax=%d", fdmax);
//               // char buf[256];
//               // SMSG("Before read");
//               // int nbytes = recv (client_fd, buf, sizeof buf, 0);
//               // SMSG("After read");
//               
//            }
//            if (fd == client_fd)
//            {
//               // check if client data is available
//               if ((nbytes = recv (fd, buf, sizeof buf, 0)) <= 0)
//               {
//                  if (nbytes == 0) {
//                     SMSG ("Cilent Connection Closed");
//                  } else {
//                     perror("Recv :: ");
//                  }
//                  close (fd);
//                  FD_CLR (fd, &master_set);
//               }
//               else {
//                  SMSG ("Read reqeust received from client");
//                  char buf[256];
//                  bzero(&buf, 256);
//                  send (client_fd, buf, 12, 0); 
//               }
//            }
//         }
//      } // for loop
