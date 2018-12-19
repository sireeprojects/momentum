#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

#define BUFSIZE 16

// Notes: 
// invoke the program like this..
// a.out -h -i 12 -p 1 -s -d
// : in the getopts mean an option with value
// i: means -i <value> 
//    value is stored in "optarg"
//    optarg is an extern char * 

int main(int argc, char *argv[]) 
{
   char c;
   char port[BUFSIZE+1];
   char addr[BUFSIZE+1];

   printf ("%s", __func__);
   
   while ( (c = getopt(argc, argv, "i:p:hs") ) != -1) 
   {
      switch (c) 
      {
         case 'i': {
             snprintf(addr,BUFSIZE,"%s",optarg);
             cout << "addr: " << addr << endl;
             break;
             }
         case 'h': {
             snprintf(addr,BUFSIZE,"%s",optarg);
             cout << "h: " << addr << endl;
             break;
             }
         case 'p': {
             snprintf(port,BUFSIZE,"%s",optarg);
             cout << "port: " << port << endl;
             break;
             }
         case 's': {
             snprintf(addr,BUFSIZE,"%s",optarg);
             cout << "s: " << addr << endl;
             break;
             }
         case '?': {
             fprintf(stderr, "Unrecognized option!\n");
             break;
             }
      }
   }
}
