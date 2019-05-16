#ifndef IPROXY_H
#define IPROXY_H

#include <iostream>
#include <stdio.h>
#include <string.h>

namespace ethTester {

using namespace std;    

typedef enum {
    TX, 
    RX
} iProxyDirection;

class iProxy {
public:
    // ctor
    iProxy(
        unsigned int id,
        string name,
        string dataPlaneSocket,
        unsigned int txBufSize,
        unsigned int rxBufSize);

    // dtor
    ~iProxy();

    // read frames from datapath socket and 
    // write to HW buffer
    unsigned int txWorker(unsigned int numElemsAvail);

    // receive frames from HW side and write into
    // datapath socket leading to the adaptor
    void rxWorker(unsigned int *data, unsigned char eom);

private:
    // print the bytes of a frame in hex format 
    // for both Tx and Rx frames
    void printFrame();


};
}

#endif
