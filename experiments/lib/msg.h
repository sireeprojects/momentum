#ifndef __MSG__
#define __MSG__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

enum msg_verbosity {
   error   = 0,
   warning = 1,
   info    = 2,
   debug   = 3
};

uint16_t verbosity = debug;

#define MSG(level,prefix,format,...) {                      \
   if (verbosity >= level) {                                \
      fprintf(stderr, prefix ": " format, ##__VA_ARGS__) ;  \
      fprintf (stderr, "\n");                               \
      fflush (stderr);                                      \
   }                                                        \
}

#define ERROR(...)   MSG(0, "[%l]ERROR   ", sc_time_stamp(), ##__VA_ARGS__);
#define WARNING(...) MSG(1, "[%l]WARNING ", sc_time_stamp(), ##__VA_ARGS__);
#define INFO(...)    MSG(2, "[%l]INFO    ", sc_time_stamp(), ##__VA_ARGS__);
#define DEBUG(...)   MSG(3, "[%l]DEBUG   ", sc_time_stamp(), ##__VA_ARGS__);

#endif
