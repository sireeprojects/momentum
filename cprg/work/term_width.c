// g++ <filename> -ltermcap

#include <stdio.h>
#include <stdlib.h>
#include <termcap.h>
#include <error.h>

static char termbuf[2048];

int main(void)
{
    char *termtype = getenv("TERM");

    if (tgetent(termbuf, termtype) < 0) {
        error(EXIT_FAILURE, 0, "Could not access the termcap data base.\n");
    }

    int lines = tgetnum( (char*) "li");
    int columns = tgetnum((char*) "co");
    // printf("lines = %d; columns = %d.\n", lines, columns);
    // printf ("\n");

    unsigned int i;
    for (i=0; i<columns; i=i+1) { printf ("-"); } printf ("\n");
    printf ("This is a line of text between llines\n");
    for (i=0; i<columns; i=i+1) { printf ("-"); } printf ("\n");

    return 0;
}
