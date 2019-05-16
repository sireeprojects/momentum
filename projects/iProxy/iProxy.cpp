#include "iProxy.h"

namespace ethTester {

int swapDataPlane(iProxy *dest, iProxy *src) {
    return 0;
}

int connectToControlPlane(string path) {
    return 0;
}

int closeControlPlane() {
    return 0;
}

iProxy::iProxy(unsigned int id, string name, string dataPlaneSocket, unsigned int txBufSize, unsigned int rxBufSize) { 
    // register name and pid of this instance
    // create dataplane
    // reserve dataplane socket buffers
    // reserve burst cache ?how to handle this in multireset scenario
    createBuffers();
}

iProxy::~iProxy() {
    delete [] txCache;
    delete [] rxCache;
}

unsigned int iProxy::txWorker(unsigned int numElemsAvail) {
    return 0;
}

void iProxy::rxWorker(unsigned int *elemData, unsigned char eom) {
}

void iProxy::reset(iProxyResetType type=SOFT) {
    switch (type) {
        case SOFT:{
            // statistics
            numFramesTransmitted = 0;
            numFramesReceived = 0;
            // end of test // RECHECK
            enEot = false;
            transmitDone = false;
            receiveDone = false;
            numTxnsForEot = 0;
            // runtime for rx worker
            rxFrameOffset = 0;
            numElemsRcvd = 0;
            break;
        }
        case HARD:{
            // proxy identifiers
            name.clear();
            pID = 0;
            // statistics
            numFramesTransmitted = 0;
            numFramesReceived = 0;
            // dataplane descriptor
            dpSock = -1;
            // end of test
            enEot = false;
            transmitDone = false;
            receiveDone = false;
            numTxnsForEot = 0;
            // runtime for rx worker
            rxFrameOffset = 0;
            numElemsRcvd = 0;
            // burst mode
            enBurst = false;
            numFramesPerBurst = 5;
            // otb pinning
            txOtbCpuID = 0;
            rxOtbCpuID = 0;
            txOtbCpuPinned = false;
            rxOtbCpuPinned = false;
            break;
        }
    }
}

void iProxy::configure(iProxyFeature feature, unsigned int value) {
}

int iProxy::connectToDataPlane(string path) {
    return 0;
}

int iProxy::setDataPlaneSocketBufSize(unsigned int txSize, unsigned int rxSize) {
    return 0;
}

int iProxy::closeDataPlane() {
    return 0;
}

void iProxy::printFrame(iProxyDirection dir, unsigned char *data, unsigned int len) {
}

void iProxy::createBuffers() {
    // CHECK: 10240 is just a placeholder size
    // actual size should be burst_size * jumbo size
    txCache = new char [10240];
    rxCache =new char [10240];
}

void iProxy::calculateLatency() {
}

void iProxy::setAutoEot(bool flag) {
    enEot = flag;
}

char *iProxy::readableFS(double size, char *buf) {
    return buf;
}

int iProxy::fdSetBlocking(int fd, int blocking) {
    return 0;
}

}
