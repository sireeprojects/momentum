#include <stdio.h> 
#include <errno.h> 

int main () {
    int a = 1/0;
    printf ("Errnum: %d", errno);
    return 0;
}
