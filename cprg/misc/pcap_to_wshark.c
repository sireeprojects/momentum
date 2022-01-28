#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define PACKED __attribute__((__packed__))

struct PACKED file_header {
    unsigned int magic           : 32;
    unsigned short version_major : 16;
    unsigned short version_minor : 16;
    unsigned int thiszone        : 32;
    unsigned int sigfigs         : 32;
    unsigned int snaplen         : 32;
    unsigned int linktype        : 32;
};

struct PACKED pkt_hdr {
    unsigned int tv_sec  : 32;
    unsigned int tv_usec : 32;
    unsigned int caplen  : 32;
    unsigned int len     : 32;
};

struct PACKED eth_pkt {
   unsigned long dst : 48;
   unsigned long src : 48;
   unsigned short lt : 16;
   char payload[46];
   unsigned int fcs : 32;
};

int main() {

    int pipe;
    struct file_header filehdr;
    struct pkt_hdr pkthdr;
    struct eth_pkt pkt;

    // open a named pipe that was already created with mkfifo
    pipe = open("/tmp/myFIFO", O_WRONLY | O_NONBLOCK);

    // set file header fields
    filehdr.magic= 0xa1b2c3d4;
    filehdr.version_major = 2;  
    filehdr.version_minor = 4;  
    filehdr.thiszone = 0;       
    filehdr.sigfigs = 0;        
    filehdr.snaplen = 4194304;
    filehdr.linktype = 1;     
    // send file header to pipe. should be done only once once
    write(pipe, (char*)&filehdr,sizeof(struct file_header));

    // set pkt header field
    pkthdr.tv_sec  = 0;
    pkthdr.tv_usec = 0;
    pkthdr.caplen = 64;
    pkthdr.len = 64;

    // set Frame fields
    pkt.dst = 0x112233445566L;
    pkt.src = 0xaabbccddeeaaL;
    pkt.lt  = 0x0008; // IPv4 (in network byte order)

    // write the pkt to wireshark
    write(pipe, (char*)&pkthdr, sizeof(struct pkt_hdr));
    write(pipe, (char*)&pkt, sizeof(struct eth_pkt));

   	// close the pipe
    close(pipe);
    return 0;
}
