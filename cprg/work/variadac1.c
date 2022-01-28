#include <stdio.h>
#include <stdarg.h>

#define IXIA_PRINT(...)                      \
   printf ("\nIxia > "__VA_ARGS__);          \
   fprintf (fp, "\nIxia > "__VA_ARGS__);     \
   fflush(stdout);                           \

#define AVIP_PRINT(...)                      \
   printf ("\nAvip > "__VA_ARGS__);          \
   fprintf (fp, "\nAvip > "__VA_ARGS__);     \
   fflush(stdout);                           \

#define VHOST_PRINT(...)                     \
   printf ("\nVhost > "__VA_ARGS__);         \
   fprintf (fp, "\nVhost > "__VA_ARGS__);    \
   fflush(stdout);                           \

int main ()
{
   FILE *fp;
   fp = fopen("file.txt","w");
   IXIA_PRINT ("This is just one argument %d", 10);
   AVIP_PRINT ("This is just one argument %d", 10);
   printf ("\n");
   fclose(fp);
   return(0);
}
