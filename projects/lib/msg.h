#ifndef __MSG__
#define __MSG__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[39m"

#define FRED(...) KRED __VA_ARGS__
#define FGRN(...) KGRN __VA_ARGS__
#define FYEL(...) KYEL __VA_ARGS__
#define FBLU(...) KBLU __VA_ARGS__
#define FMAG(...) KMAG __VA_ARGS__
#define FCYN(...) KCYN __VA_ARGS__
#define FWHT(...) KWHT __VA_ARGS__

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

char buf[1000];
uint32_t verbosity;
FILE * logfile;

static void __attribute__((constructor, used)) func(void) 
{
      logfile = fopen ("run.log", "w");
}

#define ERROR(...) {             \
      sprintf(buf, "ERROR   : ");   \
      sprintf(buf+10, __VA_ARGS__); \
      printf(FRED("%s",buf));       \
      printf("\n");              \
      fprintf (logfile, buf);       \
      fprintf (logfile, "\n");   \
      }

#define WARNING(...) {           \
      sprintf(buf, "WARNING : ");   \
      sprintf(buf+10, __VA_ARGS__); \
      printf(FYEL("%s",buf));       \
      printf("\n");              \
      fprintf (logfile, buf);       \
      fprintf (logfile, "\n");   \
      }

#define INFO(...) {              \
      sprintf(buf, "INFO    : ");   \
      sprintf(buf+10, __VA_ARGS__); \
      printf(FWHT("%s",buf));       \
      printf("\n");              \
      fprintf (logfile, buf);       \
      fprintf (logfile, "\n");   \
      }

#define DEBUG(...) {             \
      sprintf(buf, "DEBUG   : ");   \
      sprintf(buf+10, __VA_ARGS__); \
      printf(FBLU("%s",buf));       \
      printf("\n");              \
      fprintf (logfile, buf);       \
      fprintf (logfile, "\n");   \
      }

#define TITLE(...) {             \
      sprintf(buf, __VA_ARGS__); \
      printf(FBLU("%s",buf));    \
      printf(RST "\n");              \
      fprintf (logfile, buf);    \
      fprintf (logfile, "\n");   \
      }

#endif
