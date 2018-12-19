// compile:  gcc test.c -Wl,-wrap=puts

#include <stdio.h>
#include <string.h>


/* wrapping puts function */
int __wrap_puts (const char* str)
{
    /* printing out the number of characters */
    printf("puts:chars#:%lu\n", strlen(str));
    return 0;
}

int main()
{
    printf("Hello, World!\n");
    return 0;
}
