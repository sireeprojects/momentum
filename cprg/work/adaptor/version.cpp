#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

using namespace std;

int main ()
{
   FILE *fp;
   string ver_fname = "version.h";
   time_t rawtime;
   struct tm * timeinfo;
   char buffer [80];

   time (&rawtime);
   timeinfo = localtime (&rawtime);

   strftime (buffer,80,"%Y%m%d%I%M%S",timeinfo);

   fp = fopen (ver_fname.c_str(), "w");
   if (fp == NULL) 
   {
      printf ("*** Error: Unable to create version.h file\n");
      exit(1);
   }

   fprintf (fp, "unsigned long version=%llu ;" , buffer);
   fflush(fp);

  return 0;
}
