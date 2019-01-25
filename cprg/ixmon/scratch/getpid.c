#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include<stdlib.h>

int main() {
    int total_length =100;
    char line[total_length];
    FILE * command = popen("pidof proc","r");
    fgets(line,total_length,command);
    pid_t pid = strtoul(line,NULL,10);
    printf ("pid of proc: %d\n", pid);
    return 0;
}
