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
}

iProxy::~iProxy() {
}

unsigned int iProxy::txWorker(unsigned int numElemsAvail) {
    return 0;
}

void iProxy::rxWorker(unsigned int *elemData, unsigned char eom) {
}

void iProxy::reset(iProxyResetType type) {
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

void iProxy::enableAutoEot() {
}

char *iProxy::readableFS(double size, char *buf) {
    return buf;
}

int iProxy::fdSetBlocking(int fd, int blocking) {
    return 0;
}

}
