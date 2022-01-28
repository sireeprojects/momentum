#include "common.h"


int main ()
{
   struct sockaddr_un server;
   int client_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, "/tmp/seqpacket.sock");

   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
      perror("Connecting stream socket");
      close(client_fd);
      exit(1);
   }

   sleep (2);

   // shared memory
   int file_fd = 0;
   char *ptr;
   file_fd = shm_open("shm_test", O_CREAT | O_RDWR, 0777);
   ftruncate(file_fd, 8);

   ptr = (char*) mmap (0, 8, PROT_WRITE, MAP_SHARED, file_fd, 0);
   const  char *message = "12345678";
   memcpy (ptr, message, 8);

   printf ("Sending FD = %d\n", file_fd);
   send_fd (client_fd, file_fd);


   // EventFD
   // int file_fd = eventfd(0, EFD_NONBLOCK);
   // printf ("Sending FD = %d\n", file_fd);
   // send_fd (client_fd, file_fd);
   // sleep (5);
   // eventfd_write(file_fd, 1);
   // sleep (10);
   // eventfd_write(file_fd, 1);


   // // Regular FILE
   // char buf [256];
   // bzero (buf,256);
   // int file_fd = open ("file.txt", O_RDWR);
   // printf ("Sending FD = %d\n", file_fd);
   // send_fd (client_fd, file_fd);

   // sleep (5);
   // write (file_fd, buf, 1);
   // fsync (file_fd);
   // sleep (10);
   // write (file_fd, buf, 1);
   // fsync (file_fd);

   sleep (100);
}
