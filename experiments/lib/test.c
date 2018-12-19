#include <stdio.h>
#include "msg.h"

int main ()
{
   ERROR ("%s = %d", "Error Message", verbosity);
   WARNING ("Warning message");
   INFO ("Simple Info");
   DEBUG ("Detailed debug messages Info");
}
