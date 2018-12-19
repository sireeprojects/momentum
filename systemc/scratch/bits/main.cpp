#include "systemc.h"

#define MSG(x) cout << "["<< sc_time_stamp().value() <<"] "<< x << endl;

int sc_main (int argc, char *argv[])
{
   MSG ("Bit Select Exercise");

   unsigned int tmp=0;
   sc_lv<64> data = 0;
   sc_logic ltmp = sc_logic('z');

   // default
   MSG ("Data = "<< data);

   tmp = 2;
   data.range(1,0) = "1x";
   MSG ("Data = "<< data);

   MSG ("Data = "<< ltmp);

   ltmp = data[0];

   MSG ("Data = "<< ltmp);

   return 0;
}
