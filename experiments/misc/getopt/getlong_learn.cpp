#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
using namespace std;

// disable default perror from getopt_long
extern int opterr;

int main (int argc, char *argv[])
{
    opterr = 0;

    int opt = 0;
    int area = -1, perimeter = -1, breadth = -1, length =-1;

   static struct option long_options[] = {
      {"area"     , no_argument      , 0, 'a' },
      {"perimeter", no_argument      , 0, 'p' },
      {"length"   , required_argument, 0, 'l' },
      {"breadth"  , required_argument, 0, 'b' },
      {0          , 0                , 0,  0  }
   };

   while ((opt = getopt_long(argc, argv, "apl:b:", long_options, NULL)) != -1)
   {
      switch (opt)
      {
         case 'a':{
            printf ("Option a=%d\n", area);
            break;
            }
         case 'p':{
            printf ("Option p=%d\n", perimeter);
            break;
            }
         case 'l':{
            // printf ("Option l=%d\n", atoi(optarg));
            printf ("Option l=%s\n", optarg);
            break;
            }
         case 'b':{
            printf ("Option b=%d\n", atoi(optarg));
            break;
            }
         default:{
            printf ("%s\n", "Unsupported Option");
            exit(EXIT_FAILURE);
            break;
            }
      }
   }

   return 0;
}
