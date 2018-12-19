#include <stdio.h>
#include <stdlib.h>


int main( int argc, char *argv[] )
{
   FILE *fp;
   char path[1035];
   fp = popen("sysctl -n net.core.rmem_default", "r");
   if (fp == NULL) 
   {
     printf("Failed to run command\n" );
     exit(1);
   }
   fgets(path, sizeof(path)-1, fp);
   int a = atoi(path);
   printf("%d\n", a);
   pclose(fp);
   return 0;
}




// #include <stdio.h>
// #include <stdlib.h>
// 
// 
// int main( int argc, char *argv[] )
// {
// 
//   FILE *fp;
//   char path[1035];
// 
//   /* Open the command for reading. */
//   fp = popen("sysctl -n net.core.rmem_default", "r");
//   if (fp == NULL) {
//     printf("Failed to run command\n" );
//     exit(1);
//   }
// 
//   /* Read the output a line at a time - output it. */
//   while (fgets(path, sizeof(path)-1, fp) != NULL) {
//     printf("%s", path);
//   }
// 
//   /* close */
//   pclose(fp);
// 
//   return 0;
// }
