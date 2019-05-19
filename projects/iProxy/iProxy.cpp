#include "iProxy.h"

namespace ethTester {

unsigned int iProxy::cpuID = 0;

int swapDataPlane(iProxy *dest, iProxy *src) {
    return 0;
}

int connectToControlPlane(string path) {
    // create server descriptor
    struct sockaddr_un cpServer;
    cpServer.sun_family = AF_UNIX;
    strcpy(cpServer.sun_path, path.c_str());

    // create client descriptor
    cpClient =socket(AF_UNIX, SOCK_SEQPACKET, 0);

    // try to connect to control plane server
    if (connect(
            cpClient,
            (struct sockaddr*) &cpServer,
            sizeof(struct sockaddr_un)) < 0) {
        close(cpClient);
        // TODO print path along with error
        printf("Error");
        exit(1);
    }
    return 0;
}

void closeControlPlane() {
    if (close(cpClient) != 0) {
        printf("Error closing Control plane");
    }
}

iProxy::iProxy(unsigned int id,
               string name,
               string dataPlaneSocket,
               unsigned int txbufSize,
               unsigned int rxbufSize) {
    // register name and pid of this instance
    pName = name;
    pID = id;
    // create dataplane
    connectToDataPlane(dataPlaneSocket);
    // reserve dataplane socket buffers
    setDataPlaneSocketBufSize(txbufSize, rxbufSize);
    // reserve burst cache ?how to handle this in multireset scenario
    createBuffers();
    // resolve cpu id for otb pinning
    txOtbCpuID = cpuID;
    rxOtbCpuID = cpuID+1;
    cpuID += 2;
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
            // CHECK: end of test
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
            pName.clear();
            pID = 0;
            // statistics
            numFramesTransmitted = 0;
            numFramesReceived = 0;
            // dataplane descriptor
            dpClient = -1;
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
            // TODO: close socket connection
            break;
        }
    }
}

void iProxy::configure(iProxyFeature feature, unsigned int value) {
}

int iProxy::connectToDataPlane(string path) {
    // create server descriptor
    struct sockaddr_un dpServer;
    dpServer.sun_family = AF_UNIX;
    strcpy(dpServer.sun_path, path.c_str());

    // create client descriptor
    dpClient = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    // try to connect to data plane server
    if (connect(
            dpClient,
            (struct sockaddr*) &dpServer,
            sizeof(struct sockaddr_un)) < 0) {
        close(dpClient);
        // TODO print path along with error
        printf("Error");
        exit(1);
    }
    return 0;
}

int iProxy::setDataPlaneSocketBufSize(unsigned int txSize,
                                      unsigned int rxSize) {
    char buf[100];
    int bufSize;
    socklen_t bufSizeLen;
    bufSizeLen = sizeof (bufSize);

    // TODO check the return of get/setsockopt
    // display existing socket buffer sizes
    getsockopt(
        dpClient,
        SOL_SOCKET,
        SO_SNDBUF,
        &bufSize,
        &bufSizeLen);
    printf ("Current Socket TX Buffer Size: %s\n",
        readableFS (bufSize,buf));

    getsockopt(
        dpClient,
        SOL_SOCKET,
        SO_RCVBUF,
        &bufSize,
        &bufSizeLen);
    printf ("Current Socket RX Buffer Size: %s\n",
        readableFS (bufSize,buf));

    // calculate and set new socket sizes for transmit side
    bufSize = txSize*1024*1024;
    printf ("Setting Socket TX Buffer Size: %s\n",
        readableFS (bufSize,buf));
    setsockopt(
        dpClient,
        SOL_SOCKET,
        SO_RCVBUF,
        (char*) &bufSize,
        sizeof (bufSizeLen));

    // calculate and set new socket sizes for receive side
    bufSize = rxSize*1024*1024;
    printf ("Setting Socket RX Buffer Size: %s\n",
        readableFS (bufSize,buf));
    setsockopt(
        dpClient,
        SOL_SOCKET,
        SO_SNDBUF,
        (char*) &bufSize,
        sizeof (bufSizeLen));

    // check if the new sizes are set
    getsockopt(
        dpClient,
        SOL_SOCKET,
        SO_RCVBUF,
        &bufSize,
        &bufSizeLen);
    printf ("New Socket TX Buffer Size: %s\n", readableFS (bufSize,buf));

    getsockopt(
        dpClient,
        SOL_SOCKET,
        SO_SNDBUF,
        &bufSize,
        &bufSizeLen);
    printf ("New Socket RX Buffer Size: %s\n", readableFS (bufSize,buf));
    return 0;
}

int iProxy::closeDataPlane() {
    if (close(dpClient) != 0) {
        printf("Error closing Data plane");
    }
    return 0;
}

void iProxy::printFrame(iProxyDirection dir, unsigned char *data,
                        unsigned int len) {
    stringstream serialzed;
    unsigned int idx;

    serialzed.str ("");
    serialzed.setf(ios::hex, ios::basefield);

    switch (dir) {
        case TX:{
            serialzed << "Transmited Frame";
            adaptorHeader *frmHdr = new adaptorHeader;
            memcpy (frmHdr, data, sizeof (struct adaptorHeader));

            serialzed << "Frame Latency: " << frmHdr->latency << endl;
            serialzed << "Frame Size: " << frmHdr->len << endl;
            serialzed << "Frame Ipg: " << frmHdr->ipg << endl;

            for (unsigned int x=0; x<(32+8+frmHdr->len)/32; x++) {
                for (unsigned int y=0; y<32; y++) {
                    serialzed <<
                        noshowbase<<
                        setw(2)<<setfill('0')<<hex<<
                        (unsigned short)(data[idx])<<" ";
                    idx++;
                }
                serialzed << endl;
            }
            for (unsigned int x=idx; x<frmHdr->len+32+8; x++) {
                serialzed <<
                    noshowbase<<
                    setw(2)<<setfill('0')<<hex<<
                    (unsigned short) (data[idx])<<" ";
               idx++;
            }
            break;
        }
        case RX:{
            serialzed << "Received Frame";
            for (unsigned int x=0; x<len/32; x++) {
                for (unsigned int y=0; y<32; y++) {
                    serialzed <<
                        noshowbase<<
                        setw(2)<<setfill('0')<<hex<<
                        (unsigned short)(data[idx])<<" ";
                    idx++;
                }
                serialzed << endl;
            }
            for (unsigned int x=idx; x<len+32+8; x++) {
                serialzed <<
                    noshowbase<<
                    setw(2)<<setfill('0')<<hex<<
                    (unsigned short) (data[idx])<<" ";
                idx++;
            }
            break;
        }
    }
    printf("%s", serialzed.str().c_str());
    fflush (stdout);
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

// print the size which is provided in number of bytes to its equivalent
// and human readable file system format like KB, MB GB etc.
char *iProxy::readableFS(double size, char *buf) {
    memset(buf, 0, 100);
    int unitsIdx = 0;
    const char *units[] = {
        "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size>1023) {
        size /= 1024;
        unitsIdx++;
    }
    sprintf(buf, "%.0f %s", size, units[unitsIdx]);
    return buf;
}

void iProxy::fdSetBlocking(int fd, int blocking) {
    // read existing flags for the descriptor
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags==-1) {
        printf("Error getting flags");
    }
    // enable blocking/nonblocking bit in the flags
    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    // write new flags to the descriptor
    if(fcntl(fd, F_SETFL, flags) == -1){
        printf("Error setting Flags");
    }
}

void iProxy::reserveCPUforOTBThreads(unsigned int pid,
                                     iProxyDirection dir) {
    switch (dir) {
        case TX:{
            if (!txOtbCpuPinned) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(txOtbCpuID, &cpuset);
                pthread_setaffinity_np(pthread_self(),
                                       sizeof(cpu_set_t),
                                       &cpuset);
                char threadName[32];
                sprintf(threadName, "otbTX_%d", pid);
                pthread_setname_np(pthread_self(), threadName);
                txOtbCpuPinned = true;
            }
            break;
        }
        case RX:{
            if (!rxOtbCpuPinned) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(rxOtbCpuID, &cpuset);
                pthread_setaffinity_np(pthread_self(),
                                       sizeof(cpu_set_t),
                                       &cpuset);
                char threadName[32];
                sprintf(threadName, "otbRX_%d", pid);
                pthread_setname_np(pthread_self(), threadName);
                rxOtbCpuPinned = true;
            }
            break;
        }
    }
}

} // namespaces
