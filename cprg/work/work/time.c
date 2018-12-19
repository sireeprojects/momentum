/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */

int main ()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];
  char buffer1 [80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,80,"%F-%I-%M-%S",timeinfo);
  puts (buffer);
  strftime (buffer1,80,"%Y%m%d-%I%M%S",timeinfo);
  puts (buffer1);

  return 0;
}
