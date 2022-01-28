#include <iostream>
#include <stdio.h>
using namespace std;
FILE *fp;

#define IXIA_PRINT(...)             \
{                                   \
char *msg = new char [1024];        \
do {                                \
   sprintf (msg, "\n>> Ixia::");    \
   sprintf (msg+10, __VA_ARGS__);   \
   printf(msg);                     \
   fputs(msg, fp);                  \
} while (0);                        \
fflush(stdout);                     \
fflush(fp);                         \
delete [] msg;                      \
}

#define AVIP_PRINT(...)             \
{                                   \
char *msg = new char [1024];        \
do {                                \
   sprintf (msg, "\n>> Avip::");    \
   sprintf (msg+10, __VA_ARGS__);   \
   printf(msg);                     \
   fputs(msg, fp);                  \
} while (0);                        \
fflush(stdout);                     \
fflush(fp);                         \
delete [] msg;                      \
}

#define VHOST_PRINT(...)            \
{                                   \
char *msg = new char [1024];        \
do {                                \
   sprintf (msg, "\n>> Vhost::");   \
   sprintf (msg+11, __VA_ARGS__);   \
   printf(msg);                     \
   fputs(msg, fp);                  \
} while (0);                        \
fflush(stdout);                     \
fflush(fp);                         \
delete [] msg;                      \
}

#define ADAPTOR_PRINT(...)          \
{                                   \
char *msg = new char [1024];        \
do {                                \
   sprintf (msg, __VA_ARGS__);   \
   printf(msg);                     \
   fputs(msg, fp);                  \
} while (0);                        \
fflush(stdout);                     \
fflush(fp);                         \
delete [] msg;                      \
}

int main ()
{
   fp = fopen("adaptor.log", "w+");
   int a= 10;
   ADAPTOR_PRINT ("Cadence Design Systems\n");
   ADAPTOR_PRINT ("Cadence Design Systems\n");
   IXIA_PRINT ("This is just a simple text message = %d", a);
   AVIP_PRINT ("This is just a simple text message = %d", a);
   VHOST_PRINT ("This is just a simple text message = %d", a);
   printf ("\n");
   return 0;
}
