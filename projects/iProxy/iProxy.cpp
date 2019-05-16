#include "iProxy.h"

namespace ethTester {

iProxy::iProxy(unsigned int id, string name, string dataPlaneSocket, unsigned int txBufSize, unsigned int rxBufSize) {
}

iProxy::~iProxy() {
}

unsigned int iProxy::txWorker(unsigned int numElemsAvail) {
    return 0;
}

void iProxy::rxWorker(unsigned int *data, unsigned char eo) {
}

void iProxy::printFrame() {
}

}
