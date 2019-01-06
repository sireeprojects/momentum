#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#ifdef MOREMSGS
    #define DBGmsg(...) \
    printf (__VA_ARGS__); \
    fflush(stdout); 
#else
    #define DBGmsg(...) {}
#endif 

#define MAX_PORTS       256
#define RX_BUFMAX       16384
#define NUM_PORTS       1
#define NUM_TXN         100
#define PACKED          __attribute__((__packed__))
#define DATA_PLANE      "/tmp/mpktgen_dp.sock"
#define CONTROL_PLANE   "/tmp/mpktgen_cp.sock"
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define MAX_READ         10240

using namespace std;

struct PACKED ipkt {
    unsigned int ipg     :32;
    unsigned int len     :32;
    unsigned int latency :32;
    char pad [20];
    char data [4096];
};

enum txstate {
    tx_default,
    tx_pending
};

class iproxy {
public:
    void reset();
    void handle_txn_cb (unsigned int *data, unsigned char eom);
    void print_data (ipkt *p);
    void print_cdata (unsigned char* tmp, int len);
    int enqueue_frames (uint32_t nof_elems_avail);
    iproxy (uint32_t id, string name, string data_sockpath, uint32_t txsize, uint32_t rxsize); 
    ~iproxy() {}
    string pxy_name;
    uint32_t pid;
    uint32_t bytecount;
    uint32_t nof_elems_for_cur_frame;
    uint64_t txcnt;
    uint64_t rxcnt;
    // unix data plane
    int dsock;
    uint32_t dsocksize_tx;
    uint32_t dsocksize_rx;
    string data_sock_path;
    void attach_to_dsock();
    int fd_set_blocking(int fd, int blocking);
    char *readable_fs (double size, char *buf);
    void cpy64 (uint32_t *dest, uint32_t *src);
    void disable_eot ();
    // unix control plane
    static int csock;
    static void attach_to_csock(string path);
    static void detach_from_csock();
    // OTB input streaming state
    txstate state;
    // runtime variables
    ipkt pkt;
    uint32_t rxoffset;
    uint32_t rxelems;
    uint32_t nof_elems_in_buf;
    bool eom;
    uint32_t *rxbuffer;
    uint32_t *txbuffer;
    bool txdone;
    bool rxdone;
    uint32_t nof_txn;
    bool do_eot;
    //
    thread tx; 
    void tx_thread();
    char *txCache;
    char *rxCache;
    int nBytesInTxCache;
    void print_cdata16 (char* tmp, int len);
    void set_sock_bufsize (uint64_t txsize, uint64_t rxsize);
};

iproxy *proxy[MAX_PORTS];
int iproxy::csock = -1;

int iproxy::enqueue_frames(uint32_t nElemsAvail) {
    int nBytesRead = -1;
    int nBytesToProcess = -1;
    int n64ByteElems = -1;
    int nElemsToSend = -1;
    int head = 0;

    // try to read data from dplane socket
    nBytesRead = read(dsock, txCache+head, MAX_READ);
    // proceed only if we have data to process
    if (nBytesRead>0) {
        nBytesToProcess = nBytesRead+nBytesInTxCache;
        DBGmsg("nBytesRead: %d\n", nBytesRead);
        // DBGmsg("nBytesInTxCache: %d\n", nBytesInTxCache);
        // find number of 64 byte elems to send
        n64ByteElems = nBytesToProcess/64; 
        // DBGmsg("n64ByteElems: %d\n", n64ByteElems);
        nElemsToSend = MIN(nElemsAvail, n64ByteElems);
        // DBGmsg("nElemsToSend: %d\n", nElemsToSend);
        // TODO try and simplify this logic
        nBytesInTxCache = nBytesToProcess - (nElemsToSend*64);
        // DBGmsg("nBytesInTxCache: %d\n", nBytesInTxCache);
        // send data to bfm
        for (int nElems=0; nElems<nElemsToSend; nElems++) {
            //TODO: print/send data
            // print_cdata16((txCache+head),64);
            head += 64;
        }
        // handle residue
        memcpy(txCache, txCache+(nElemsToSend*64), nBytesToProcess-(nElemsToSend*64));
    }
}

void iproxy::cpy64(uint32_t *dest, uint32_t *src) {
    for (int i=0; i<16; i++) {
        *(dest+i) = src[i];
    }
}

void iproxy::handle_txn_cb(unsigned int *data, unsigned char eom) {
    cpy64(rxbuffer+rxoffset, data);
    rxoffset += 16;
    rxelems++;

    if ((eom&0x01)==1) {
        write(dsock, (char*) rxbuffer, rxelems*64);
        rxcnt++;
        rxelems = 0;
        rxoffset = 0;
        DBGmsg("*Port:%d Received a frame: NofTxn: %lu\n", pid, rxcnt);

        if (rxcnt==nof_txn) {
            printf("Receive complete: port[%d] : %lu\n", pid, rxcnt);
            fflush(stdout);
        }
    }
}

// int iproxy::enqueue_frames (uint32_t nof_elems_avail) {
//     uint32_t n=0;
//     int nof_bytes = 0;
//     uint32_t nof_elems_consumed = 0;
//     uint32_t nof_elems_cnt = nof_elems_avail;
// 
//     if (txdone)
//         return 1;
// 
//     while (true) {
//         switch (state) {
//             case tx_default:{
//                 pkt={};
//                 // try to get a frame from IxVerify data socket
//                 nof_bytes = read (dsock, (unsigned char*) &pkt, 10240);
//                 DBGmsg ("Nof Bytes Read from Socket: %d\n", nof_bytes);
// 
//                 if (nof_bytes > 0) {
//                     if (txcnt==0)
//                         pkt.ipg = 12;
//                     pkt.len = pkt.len+4; //crc
//                     // printf ("IPG = %d\n", pkt.ipg); fflush (stdout);
//                     nof_elems_for_cur_frame = ceil ((float) (32+pkt.len)/64);
//                     DBGmsg ("Nof Elems for this frame: %d\n", nof_elems_for_cur_frame);
// 
//                     if (nof_elems_cnt >= nof_elems_for_cur_frame) {
//                         for (uint32_t i=0; i<nof_elems_for_cur_frame; i++) {
//                             // DataQ_put ((uint32_t*) &pkt+(i*16));
//                         }
//                         nof_elems_consumed += nof_elems_for_cur_frame;
//                         nof_elems_cnt = nof_elems_cnt - nof_elems_for_cur_frame;
//                         txcnt++;
//                         DBGmsg ("Nof frames transmitted: %lu\n", txcnt);
//                         if (txcnt==nof_txn) {
//                             printf ("Transmit complete: port[%d] : %lu\n", pid, txcnt);
//                             fflush (stdout);
//                             txdone = true;
//                             return 1; 
//                         }
//                         state = tx_default;
//                     } else {
//                         DBGmsg ("Port:%d: Unable to send frame\n", pid);
//                         state = tx_pending;
//                         return 0;
//                     }
//                 } else {
//                     DBGmsg ("Input Stream State: No data in Socket\n");
//                     state = tx_default;
//                     return 0;
//                 }
//                 break;
//             }
// 
//             case tx_pending:{ 
//                 DBGmsg ("Input Stream State: Pending\n");
//                 nof_elems_for_cur_frame = ceil ((float) (32+pkt.len)/64);
//                 if (nof_elems_cnt >= nof_elems_for_cur_frame) {
//                     for (uint32_t i=0; i<nof_elems_for_cur_frame; i++) {
//                         // DataQ_put ((uint32_t*) &pkt+(i*16));
//                     }
//                     nof_elems_consumed += nof_elems_for_cur_frame;
//                     nof_elems_cnt = nof_elems_cnt - nof_elems_for_cur_frame;
//                     txcnt++;
//                     DBGmsg ("Nof frames transmitted: %lu\n", txcnt);
//                     if (txcnt==nof_txn) {
//                         printf ("Transmit complete: port[%d] : %lu\n", pid, txcnt);
//                         fflush (stdout);
//                         txdone = true;
//                         return 1; 
//                     }
//                     state = tx_default;
//                 } else {
//                     DBGmsg ("Port:%d: Unable to send frame\n", pid);
//                     state = tx_pending;
//                 }
//                 break;
//             }
//         }
//     }
//     return 0;
// }

void iproxy::disable_eot() {
    do_eot = false;
}

void iproxy::reset() {
    eom = 0;
    pid = 0;
    txcnt = 0;
    rxcnt = 0;
    dsock = -1;
    rxelems = 0;
    nof_txn = 0;
    rxoffset = 0;
    do_eot = true;
    txdone = false;
    rxdone = false;
    state = tx_default;
    nof_elems_in_buf = 0;
    data_sock_path.clear();
    // rxbuffer = new uint32_t [1024];
    // txbuffer = new uint32_t [1024];
    nBytesInTxCache = 0;
}

void iproxy::attach_to_dsock() {
    struct sockaddr_un server;
    dsock = socket (AF_UNIX, SOCK_STREAM, 0);
    server.sun_family = AF_UNIX;
    strcpy (server.sun_path, data_sock_path.c_str());

    if (connect (dsock, (struct sockaddr*) &server, sizeof (struct sockaddr_un)) < 0) {
        char msg[64];
        sprintf(msg, "Data Plane Socket Connection (%d)", pid);
        perror (msg);
        close (dsock);
        exit (1);
    }
    set_sock_bufsize (0,0);
    // fd_set_blocking (dsock, 0); // nb
}

void iproxy::attach_to_csock (string path) {
    struct sockaddr_un server;
    csock = socket (AF_UNIX, SOCK_SEQPACKET, 0);
    server.sun_family = AF_UNIX;
    strcpy (server.sun_path, path.c_str());

    if (connect (csock, (struct sockaddr*) &server, sizeof (struct sockaddr_un)) < 0) {
        close (csock);
        perror ("Control Plane Socket Connection");
        exit (1);
    }
}

int iproxy::fd_set_blocking (int fd, int blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return 0;
    if (blocking) flags &= ~O_NONBLOCK;
    else flags |= O_NONBLOCK;
    return fcntl (fd, F_SETFL, flags) != -1;
}

void iproxy::detach_from_csock () {
    uint32_t len=10;
    send (csock, (char*) &len, 1, 0);
}

char *iproxy::readable_fs (double size, char *buf) {
    int i = 0;
    const char* units[] = {"B","kB","MB","GB","TB","PB","EB","ZB","YB"};

    memset (buf,0,100);

    while (size > 1023) {
        size /= 1024;
        i++;
    }
    sprintf (buf, "%.0f %s", size, units[i]);
    return buf;
}

// ctor
iproxy::iproxy(uint32_t id, string name, string data_sockpath, uint32_t txsize, uint32_t rxsize) {
    reset();
    pxy_name = name;
    data_sock_path = data_sockpath;
    dsocksize_tx = txsize;
    dsocksize_rx = rxsize;
    pid = id;
    attach_to_dsock();
    //
    free(txCache);
    free(rxCache);
    txCache = new char [10240];
    rxCache = new char [10240];
    tx = thread(&iproxy::tx_thread, this);
    char thread_name[80];
    sprintf(thread_name, "avipTxW%d", pid);
    pthread_setname_np(tx.native_handle(), thread_name);
    tx.detach();
}

void iproxy::tx_thread()
{
    sleep(15);
    while(1) {
        enqueue_frames(1024);
    } 
}

void iproxy::print_data (ipkt *p) {
    stringstream s;
    s.str ("");
    s.setf(ios::hex, ios::basefield);
    uint32_t idx = 0;
    unsigned char *tmp = new unsigned char [sizeof (struct ipkt)];
    memcpy (tmp, p, sizeof (struct ipkt));

    for (uint32_t x=0; x<(32+8+p->len)/32; x++) {
        for (uint32_t y=0; y<32; y++) {
            s <<noshowbase<<setw(2)<<setfill('0')<<hex<<uint16_t(tmp[idx])<<" ";
            idx++;
        }
        s << endl;
    }
    for (uint32_t x=idx; x<p->len+32+8; x++) {
       s<<noshowbase<<setw(2)<<setfill('0')<<hex<<uint16_t(tmp[idx])<<" ";
       idx++;
    }
    printf ("PKT Latency : %d\n" , p->latency);
    printf ("PKT Size    : %d\n" , p->len);
    printf ("PKT Ipg     : %d\n" , p->ipg);
    cout << "PKT Data :" << endl << s.str()<<endl;
    fflush (stdout);
}

void iproxy::print_cdata (unsigned char* tmp, int len) {
    stringstream s;
    s.str("");
    uint32_t idx = 0;

    printf ("PKT len : %d\n", len);
    fflush (stdout);

    for (int x=0; x<len/32; x++) {
        for (int y=0; y<32; y++) {
            s <<noshowbase<<setw(2)<<setfill('0')<<hex<<uint16_t(tmp[idx])<<" ";
            idx++;
        }
        s << endl;
    }
    for (int x=idx; x<len; x++) {
       s<<noshowbase<<setw(2)<<setfill('0')<<hex<<uint16_t(tmp[idx])<<" ";
       idx++;
    }
    cout << "PKT Data :" << endl << s.str()<<endl;
    fflush (stdout);
}

void iproxy::set_sock_bufsize (uint64_t txsize, uint64_t rxsize) {
    char buf[100];
    int bufsize;
    socklen_t bufsize_len;
    bufsize_len = sizeof (bufsize);
    getsockopt (dsock, SOL_SOCKET, SO_SNDBUF, &bufsize, &bufsize_len);
    printf ("Current Socket TX Buffer Size: %s\n", readable_fs (bufsize,buf));
    getsockopt (dsock, SOL_SOCKET, SO_RCVBUF, &bufsize, &bufsize_len);
    printf ("Current Socket RX Buffer Size: %s\n", readable_fs (bufsize,buf));
    // bufsize = txsize*1024*1024;
    // printf ("Setting Socket TX Buffer Size: %s\n", readable_fs (bufsize,buf));
    // setsockopt (dsock, SOL_SOCKET, SO_RCVBUF, (char*) &bufsize, sizeof (bufsize_len));
    // bufsize = rxsize*1024*1024;
    // printf ("Setting Socket RX Buffer Size: %s\n", readable_fs (bufsize,buf));
    // setsockopt (dsock, SOL_SOCKET, SO_SNDBUF, (char*) &bufsize, sizeof (bufsize_len));
    // getsockopt (dsock, SOL_SOCKET, SO_RCVBUF, &bufsize, &bufsize_len);
    // printf ("New Socket TX Buffer Size:     %s\n", readable_fs (bufsize,buf));
    // getsockopt (dsock, SOL_SOCKET, SO_SNDBUF, &bufsize, &bufsize_len);
    // printf ("New Socket RX Buffer Size:     %s\n", readable_fs (bufsize,buf));
}

void iproxy::print_cdata16 (char* tmp, int len) {
    uint32_t idx = 0;
    int plen = 16;
    printf ("Segment:\n");

    for (int x=0; x<len/plen; x++) {
        for (int y=0; y<plen; y++) {
            printf("%02x ", (0x00ff) & (uint16_t)tmp[idx]);
            if (idx%8==7) printf("  ");
            idx++;
        }
        printf("\n");
    }
    printf("\n");
    fflush (stdout);
}

void setup_proxy(unsigned int pid) {
    char name[32];
    sprintf(name, "port%d", pid);
    proxy[pid] = new iproxy(pid, name, DATA_PLANE, 64, 1000);
    proxy[pid]->nof_txn = NUM_TXN;
}

int setup_tb() {
    iproxy::attach_to_csock(CONTROL_PLANE);
    return 0;
}

int main() {
    setup_tb();
    for (int i=0; i<NUM_PORTS; i++) {
        setup_proxy(i);
    }
    sleep(100);
    return 0;
}
