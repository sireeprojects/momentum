#ifdef __TCPLIB__
#else


#include <stdio.h>
#include <stdint.h>


struct mac 
{
   uint64_t dest           : 48;
   uint64_t src            : 48;
   uint64_t type           : 16;
}
__attribute__((__packed__));


struct ip
{
   uint32_t ver            : 4;
   uint32_t hlen           : 4;
   uint32_t tos            : 8;
   uint32_t len            : 16;
   uint32_t id             : 16;
   uint32_t flags          : 3;
   uint32_t frag_offset    : 13;
   uint32_t ttl            : 8;
   uint32_t proto          : 8;
   uint32_t checksum       : 16;
   uint32_t src_ip         : 32;
   uint32_t dest_ip        : 32;
}
__attribute__((__packed__));


struct tcp
{
   uint32_t src_port       : 16;
   uint32_t dest_port      : 16;
   uint32_t sequence_num   : 32;
   uint32_t ack_num        : 32;
   uint32_t hlen           : 4;
   uint32_t reserved       : 6;
   uint32_t urg            : 1;
   uint32_t ack            : 1;
   uint32_t psh            : 1;
   uint32_t rst            : 1;
   uint32_t syn            : 1;
   uint32_t fin            : 1;
   uint32_t win_size       : 16;
   uint32_t checksum       : 16;
   uint32_t urg_ptr        : 16;
}
__attribute__((__packed__));


struct enet_frame 
{
   struct mac mac;
   struct ip  ip;
   struct tcp tcp;
}
__attribute__((__packed__));


struct pseudo_ip_hdr
{
   uint32_t src            : 32;
   uint32_t dest           : 32;
   uint32_t reserved       : 8;
   uint32_t proto          : 8;
   uint32_t len            : 16;
}
__attribute__((__packed__));


void print_array (uint8_t *array, uint32_t size);
void print_frame (struct enet_frame *frame);


#endif
