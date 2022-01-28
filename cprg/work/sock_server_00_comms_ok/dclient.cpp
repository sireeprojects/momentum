#include "common.h"

// #define SOCKNAME "/tmp/vubr.sock"
#define SOCKNAME "/tmp/avip_data_path.sock"

int main ()
{
   control_msg cm;
   // char buf[256] = "Hello from Client !!!\0";
   char buf[256];
   bzero(&buf, 256);
   struct sockaddr_un server;
   int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, SOCKNAME);

   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
       close(client_fd);
       CMSG("Socket Error");
       exit(1);
   }

   for (int i=0; i<1; i++)
   {
   sleep(1);
   cm.type = 5;
   cm.reserved = 0xaabbccdd11223344;
   send (client_fd, (char*)&cm, 12, 0);
   ring_info r = {};
   int nbytes = recv (client_fd, (char*)&r, sizeof r, 0);
   // int nbytes = recv (client_fd, buf, sizeof buf, 0);
   // printf ("%s\n", buf);
   printf ("====================================\n");
   print_data (&r);
   }
   sleep(100);
   return 0;
}
