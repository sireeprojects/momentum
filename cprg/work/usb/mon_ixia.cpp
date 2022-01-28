#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "usboeSocket.h"

using namespace std;

int main()
{
   usboeSocket sock;
   sock.connect_to_server((char*)"10.253.64.75", 8888);
   int len;
   char *buf = new char [10240];
   while (1)
   {
      len = sock.read_data(buf);
      if (len)
      {
         cout << "len= " << dec << len << endl;
      }
   }
   sock.close_client ();
}
