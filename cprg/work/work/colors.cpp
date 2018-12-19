#include <iostream>
#include "colors.h"
using namespace std;

int main ()
{
   cout << FRED("I'm blue.") << endl;
   cout << FGRN("I'm blue.") << endl;
   cout << FYEL("I'm blue.") << endl;
   cout << FBLU("I'm blue.") << endl;
   cout << FMAG("I'm blue.") << endl;
   cout << FCYN("I'm blue.") << endl;
   cout << FWHT("I'm blue.") << endl;
   
   cout << BOLD(FRED("I'm blue.")) << endl;
   cout << BOLD(FGRN("I'm blue.")) << endl;
   cout << BOLD(FYEL("I'm blue.")) << endl;
   cout << BOLD(FBLU("I'm blue.")) << endl;
   cout << BOLD(FMAG("I'm blue.")) << endl;
   cout << BOLD(FCYN("I'm blue.")) << endl;
   cout << BOLD(FWHT("I'm blue.")) << endl;
   cout << RST << endl;
   cout << "normal text" << endl;
   return 0;
}
