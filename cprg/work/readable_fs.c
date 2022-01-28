#include <stdio.h>


char* readable_fs(double size/*in bytes*/, char *buf) {
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1023) {
      size /= 1024;
      i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

int main ()
{
   // cout << "size = " << dec << sizeof (struct vring) << endl;
   // cout << "size = " << readable_fs (sizeof (struct vring), buf)<< endl;
   char buf[100];
   readable_fs (1024, buf);
   printf ("Size = %s", buf);
   return 0;
}
