#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sched.h>

int main () {
    // create a socket SOCK_SEQPACKET
    // connect to the control plane socket of adaptor as client
    // in a while loop start checking keyboard input
    //      if keypress is 'p' send PLAY command
    //      if keypress is 's' send STOP command
    return 0;
}
