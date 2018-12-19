#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#define wdump(signal) sc_trace (waves, signal, #signal);

ofstream myfile;
stringstream runlog;

#define MSG(...) {             \
   runlog.str("");             \
   runlog   << "["             \
            << sc_time_stamp() \
            <<"] "             \
           << id()             \
           << ": "             \
           << __VA_ARGS__      \
           << endl;            \
   cout << runlog.str();       \
   myfile << runlog.str();     \
}

string id() {
   return "top";
}

void open_logfile() {
   myfile.open ("run.log");;
}

void close_logfile() {
   myfile.close();
}


/*
#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdint.h>

enum msg_verbosity 
{
   none    = 0 ,
   low     = 1 ,
   high    = 2 ,
   full    = 3 ,
   warning = 4 ,
   error   = 5 ,
   fatal   = 6
};

// messagin macro with verbosity and file logging option
#define msg(verbosity,...)                \
{                                         \
   if (verbosity <= global_verosity)      \
   {                                      \
      printf (__VA_ARGS__);               \
      printf ("\n");                      \
      if (create_log)                     \
      {                                   \
         fprintf (logfile, __VA_ARGS__);  \
         fprintf (logfile, "\n");         \
      }                                   \
      fflush(stdout);                     \
   }                                      \
}

// Command line switch global variables
uint32_t global_verosity = 2;
uint32_t create_log = 0;

// File to dump messages from msg macro
FILE * logfile;

#endif
*/
