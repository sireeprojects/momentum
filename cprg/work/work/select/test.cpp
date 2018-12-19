#include <iostream>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int fint;
int fdmax;
fd_set master;

int main () 
{
   FILE *fp;
   fint = -1;
   fdmax = -1;

   int cnt = 0;

   fp=fopen("p.dummy", "wb");
   fint = fileno(fp); 
   FD_ZERO(&master);

   printf ("File desc number = %d\n", fint);

   FD_SET (fint, &master);
   fdmax = fint;

   while (1)
   {
      unsigned int timeout = 200000;
      struct timeval tv; 
      tv.tv_sec = timeout / 10000000000;
      tv.tv_usec = timeout % 100000;

      fd_set fdtmp = master;

      // if (select (fdmax+1, NULL, NULL, NULL, &tv) == -1)
      if (select (1, NULL, NULL, NULL, &tv) == -1)
      {
         perror ("Select->This should not happen");
         exit(4);
      }
     
      int sock;
      for (sock=0; sock<=fdmax; sock++)
      {
         printf ("[%d] Inside loop = %d\n", cnt, sock);
         if (FD_ISSET(sock, &fdtmp) && FD_ISSET(sock, &master))
         {
            printf ("Sock event sock = %d\n", sock);
         }
      } // for
      FD_ZERO(&fdtmp);
      cnt = cnt + 1;
   } // while


   return 0;
}
