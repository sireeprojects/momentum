#ifndef IPROXY_H
#define IPROXY_H

#include <iostream>
#include <stdio.h>
#include <string.h>

namespace ethTester {
using namespace std;
class iProxy;

typedef enum {TX,RX} iProxyDirection; 
typedef enum {SOFT,HARD} iProxyResetType; 
typedef enum {CONNECT, DISCONNECT} cpCommand;
typedef enum {FEAT1,FEAT2} iProxyFeature; 

// used to swap the dataplane descriptors
// during dynamic reset or speed change 
int swapDataPlane(iProxy *dest, iProxy *src);

// create a control plane between this proxy instance and adaptor
// control and status will be exchanged in this plane
// This should be the first called made from the porxy/bfm before any other action
int connectToControlPlane(string path);

// notify the adaptor that the test has ended
// adaptor uses this notification to release all the 
// resources that was reserved for the test. 
// this should be called just before c_stop_sim
int closeControlPlane();

// file descriptor of the control plane
int cpSock = -1;

class iProxy {

public:

    // ctor
    iProxy(
        // instance number of the bfm 
        unsigned int id,         
        // a string identified that will be used in messages
        string name,             
        // rtl path to the bfm that this instance will be connected to
        string dataPlaneSocket,  
        // size of the transmit dataplane
        unsigned int txBufSize,  
        // size of the receive datapath
        unsigned int rxBufSize); 

    // dtor
    ~iProxy();

    // read frames from datapath socket and 
    // write to HW buffer
    unsigned int txWorker(unsigned int numElemsAvail);

    // receive frames from HW side and write into
    // datapath socket leading to the adaptor
    void rxWorker(unsigned int *elemData, unsigned char eom);

    // soft: retains all the statistics and updates continue when
    //       resumed shud be used in multi reset and speed change tests
    // hard: bring the proxy instance to its default state
    void reset(iProxyResetType type);

    void configure(iProxyFeature feature, unsigned int value);

private:

    // create a dataplane between this proxy instance and adaptor
    // frames will be exchanged in the plane
    int connectToDataPlane(string path);

    // reserve buffer for both dataplane and control plane transfers
    int setDataPlaneSocketBufSize(unsigned int txSize, unsigned int rxSize);

    // notify adaptor that this proxy instance is going to be 
    // deleted and hence release the resources that was reserved 
    // by this proxy instance. will be called before a proxy instance 
    // is deleted
    int closeDataPlane();

    // print the bytes of a frame in hex 
    // format for both Tx and Rx frames
    // available in debug build
    void printFrame(iProxyDirection dir, unsigned char *data, unsigned int len);

    // can be used in loopback test. an end of signal will be
    // sent to bfm when the number of frames matches on both 
    // tx and rx sides. the num transaction should also be set
    // available in debug build
    void enableAutoEot();

    // convert size in bytes to filesystem format, KB/MB/GB etc
    char *readableFS(double size, char *buf);

    // use to put the dataplane in blocking or nonblocking mode
    int fdSetBlocking(int fd, int blocking);
};

}

#endif
