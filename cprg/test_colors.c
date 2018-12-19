#include <stdio.h>
#include "colors.h"

int main ()
{
   printf (FRED("Red color\n"));
   printf (FYEL("Yellow color\n"));
   printf (RST);
   printf ("Normal color\n");
   printf (FRED("I'm blue\n"));
   printf (FGRN("I'm blue\n"));
   printf (FYEL("I'm blue\n"));
   printf (FBLU("I'm blue\n"));
   printf (FMAG("I'm blue\n"));
   printf (FCYN("I'm blue\n"));
   printf (FWHT("I'm blue\n"));
   printf (BOLD(FRED("I'm blue\n")));
   printf (BOLD(FGRN("I'm blue\n")));
   printf (BOLD(FYEL("I'm blue\n")));
   printf (BOLD(FBLU("I'm blue\n")));
   printf (BOLD(FMAG("I'm blue\n")));
   printf (BOLD(FCYN("I'm blue\n")));
   printf (BOLD(FWHT("I'm blue\n")));
   return 0;
}
