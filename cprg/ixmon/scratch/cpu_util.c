#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

int main(void) {
    long double a[4], b[4], loadavg;
    int avg;
    FILE *fp;
    char dump[50];

    // prepare screen
    initscr();
    curs_set(FALSE);
    noecho();

    for(;;) {
        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
        fclose(fp);
        usleep(600000);

        fp = fopen("/proc/stat","r");
        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
        fclose(fp);

        // loadavg = (((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]) )) * 100;
        avg = (((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]) )) * 100;
        mvprintw(0,0, "CPU: %3d", avg);
        refresh();
    }
    return(0);
}
