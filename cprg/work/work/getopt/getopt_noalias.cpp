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

   enum {
      opt_area = 1000,
      opt_perimeter,
      opt_length,
      opt_breadth
   };

   static struct option long_options[] = {
      {"area"     , no_argument      , 0, opt_area },
      {"perimeter", no_argument      , 0, opt_perimeter },
      {"length"   , required_argument, 0, opt_length },
      {"breadth"  , required_argument, 0, opt_breadth },
      {0          , 0                , 0, 0 }
   };

   // while ((opt = getopt_long(argc, argv, "apl:b:", long_options, NULL)) != -1)
   while ((opt = getopt_long_only(argc, argv, "", long_options, NULL)) != -1)
   {
      switch (opt)
      {
         case opt_area:{
            printf ("Option Area\n");
            break;
            }
         case opt_perimeter:{
            printf ("Option Perimeter\n");
            break;
            }
         case opt_length:{
            printf ("Option length=%d\n", atoi(optarg));
            break;
            }
         case opt_breadth:{
            printf ("Option breadth=%d\n", atoi(optarg));
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
