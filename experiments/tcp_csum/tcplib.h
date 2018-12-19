#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_PER_LOOP 16

#define PACKED __attribute__((__packed__))

/* Frame */
static const unsigned char pkt1[84] = {
   0x08, 0x00, 0x27, 0x38, 0xdb, 0xed, 0x08, 0x00,
   0x27, 0x97, 0x3f, 0x45, 0x08, 0x00, 0x45, 0x00,
   0x00, 0x46, 0x00, 0xec, 0x40, 0x00, 0x80, 0x06,
   0xf5, 0xc1, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01,
   0x01, 0x01, 0x04, 0x12, 0x00, 0x35, 0xd1, 0xf8,
   0xc1, 0x17, 0x5f, 0xf5, 0xa8, 0xbd, 0x50, 0x18,
   0xfb, 0x90, 0x05, 0x68, 0x00, 0x00, 0x00, 0x1c,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x04, 0x65, 0x74, 0x61,
   0x73, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0xfc,
   0x00, 0x01, 0x4d, 0x53                         
};

/* pseudo header */
static const uint8_t pseudo_hdr[12]={
   0x01, 0x01, 0x01, 0x02,   /* source ip       */
   0x01, 0x01, 0x01, 0x01,   /* destination ip  */
   0x00,                     /* reserved        */
   0x06,                     /* protocol id     */
   0x32, 0x00                /* tcp length = 50 */
};

struct PACKED mac {
   uint64_t dest : 48;
   uint64_t src  : 48;
   uint64_t type : 16;
};

struct PACKED ip {
   uint32_t ver      : 4;
   uint32_t hlen     : 4;
   uint32_t tos      : 8;
   uint32_t len      : 16;
   uint32_t id       : 16;
   uint32_t flags    : 3;
   uint32_t offset   : 13;
   uint32_t ttl      : 8;
   uint32_t proto    : 8;
   uint32_t checksum : 16;
   uint32_t src_ip   : 32;
   uint32_t dest_ip  : 32;
};

struct PACKED tcp {
   uint32_t src_port  : 16;
   uint32_t dest_port : 16;
   uint32_t seq_num   : 32;
   uint32_t ack_num   : 32;
   uint32_t hlen      : 4;
   uint32_t reserved  : 6;
   uint32_t urg       : 1;
   uint32_t ack       : 1;
   uint32_t psh       : 1;
   uint32_t rst       : 1;
   uint32_t syn       : 1;
   uint32_t fin       : 1;
   uint32_t win_size  : 16;
   uint32_t checksum  : 16;
   uint32_t urg_ptr   : 16;
};

struct PACKED enet_frame {
   struct mac mac;
   struct ip  ip;
   struct tcp tcp;
   uint8_t data[30];
};

struct PACKED pseudo_ip_hdr {
   uint32_t src      : 32;
   uint32_t dest     : 32;
   uint32_t reserved : 8;
   uint32_t proto    : 8;
   uint32_t len      : 16;
};

struct PACKED tcp_checksum_data {
   struct pseudo_ip_hdr pseudo_ip_hdr;
   struct tcp tcp;
};

void print_frame_bytes (uint8_t *array, uint32_t size)
{
   uint32_t i, j, cntr=0;
   uint32_t bytes_per_loop = BYTES_PER_LOOP;
   uint32_t loop_size = size/bytes_per_loop;
   uint32_t last_loop = size%bytes_per_loop;

   printf ("Data:\n");

   for (i=0; i<loop_size; i=i+1) {
      for (j=0; j<bytes_per_loop; j=j+1) {
         printf ("%02x ", array[cntr]);
         cntr=cntr+1;
         if (j%8==7)
            printf (" ");
      }
      printf ("\n");
   }

   for (i=0; i<last_loop; i=i+1) {
      printf ("%02x ", array[cntr]);
      cntr=cntr+1;
   }

   printf ("\n\n");
}

uint64_t byte_reverse (uint64_t original, uint32_t num)
{
   uint64_t reversed = 0;
   uint32_t loopVar;
   for (loopVar=0; loopVar<num; loopVar++) {
      reversed = (reversed <<8) | (original & 0xFF);
      original = original >> 8;
   }
   return reversed;
}

static uint16_t compute_tcp_checksum_16b (uint16_t *data, uint32_t len)
{
   uint64_t sum = 0;

   while (len>1) {
      sum += *data++;
      len -= 2;
   }

   if (len>0)
      sum += ((*data)&htons (0xff00));
   
   while (sum>>16)
      sum = (sum & 0xffff) + (sum>>16);

   sum = ~sum;
   return (uint16_t) sum;
}

uint16_t compute_tcp_checksum (uint8_t *data8, uint32_t len)
{
   uint16_t *data16;
   return (compute_tcp_checksum_16b (data16,len));
}

void pack_frame_netw (struct enet_frame *frame)
{
   frame->mac.dest         =   byte_reverse (frame->mac.dest        , 6); 
   frame->mac.src          =   byte_reverse (frame->mac.src         , 6); 
   frame->mac.type         =   byte_reverse (frame->mac.type        , 2); 
   // frame->ip.ver           =   byte_reverse (frame->ip.ver          , 0); 
   // frame->ip.hlen          =   byte_reverse (frame->ip.hlen         , 0); 
   frame->ip.tos           =   byte_reverse (frame->ip.tos          , 0); 
   frame->ip.len           =   byte_reverse (frame->ip.len          , 0); 
   frame->ip.id            =   byte_reverse (frame->ip.id           , 0); 
   frame->ip.flags         =   byte_reverse (frame->ip.flags        , 0); 
   frame->ip.offset        =   byte_reverse (frame->ip.offset       , 0); 
   frame->ip.ttl           =   byte_reverse (frame->ip.ttl          , 0); 
   frame->ip.proto         =   byte_reverse (frame->ip.proto        , 0); 
   frame->ip.checksum      =   byte_reverse (frame->ip.checksum     , 0); 
   frame->ip.src_ip        =   byte_reverse (frame->ip.src_ip       , 0); 
   frame->ip.dest_ip       =   byte_reverse (frame->ip.dest_ip      , 0); 
   frame->tcp.src_port     =   byte_reverse (frame->tcp.src_port    , 0); 
   frame->tcp.dest_port    =   byte_reverse (frame->tcp.dest_port   , 0); 
   frame->tcp.seq_num =   byte_reverse (frame->tcp.seq_num, 0); 
   frame->tcp.ack_num      =   byte_reverse (frame->tcp.ack_num     , 0); 
   frame->tcp.hlen         =   byte_reverse (frame->tcp.hlen        , 0); 
   frame->tcp.reserved     =   byte_reverse (frame->tcp.reserved    , 0); 
   frame->tcp.urg          =   byte_reverse (frame->tcp.urg         , 0); 
   frame->tcp.ack          =   byte_reverse (frame->tcp.ack         , 0); 
   frame->tcp.psh          =   byte_reverse (frame->tcp.psh         , 0); 
   frame->tcp.rst          =   byte_reverse (frame->tcp.rst         , 0); 
   frame->tcp.syn          =   byte_reverse (frame->tcp.syn         , 0); 
   frame->tcp.fin          =   byte_reverse (frame->tcp.fin         , 0); 
   frame->tcp.win_size     =   byte_reverse (frame->tcp.win_size    , 0); 
   frame->tcp.checksum     =   byte_reverse (frame->tcp.checksum    , 0); 
   frame->tcp.urg_ptr      =   byte_reverse (frame->tcp.urg_ptr     , 0); 
}

uint8_t* pack_frame_host (struct enet_frame *frame)
{
}

void print_frame (struct enet_frame *frame)
{
   printf ("---------------------------------------------------------------------------\n");
   printf ("Frame Headers\n");
   printf ("---------------------------------------------------------------------------\n");
   printf ("mac.dest = %-12llx   ip.ver      = %-8llx   tcp.src_port     = %-8llx\n", frame->mac.dest,frame->ip.ver , frame->tcp.src_port     );
   printf ("mac.src  = %-12llx   ip.hlen     = %-8llx   tcp.dest_port    = %-8llx\n", frame->mac.src ,frame->ip.hlen, frame->tcp.dest_port    );
   printf ("mac.type = %-12llx   ip.tos      = %-8llx   tcp.seq_num      = %-8llx\n", frame->mac.type,frame->ip.tos , frame->tcp.seq_num );
   printf ("                          ip.len      = %-8llx   tcp.ack_num      = %-8llx\n" , frame->ip.len           , frame->tcp.ack_num  ); 
   printf ("                          ip.id       = %-8llx   tcp.hlen         = %-8llx\n" , frame->ip.id            , frame->tcp.hlen     ); 
   printf ("                          ip.flags    = %-8llx   tcp.reserved     = %-8llx\n" , frame->ip.flags         , frame->tcp.reserved ); 
   printf ("                          ip.offset   = %-8llx   tcp.urg          = %-8llx\n" , frame->ip.offset        , frame->tcp.urg      ); 
   printf ("                          ip.ttl      = %-8llx   tcp.ack          = %-8llx\n" , frame->ip.ttl           , frame->tcp.ack      ); 
   printf ("                          ip.proto    = %-8llx   tcp.psh          = %-8llx\n" , frame->ip.proto         , frame->tcp.psh      ); 
   printf ("                          ip.checksum = %-8llx   tcp.rst          = %-8llx\n" , frame->ip.checksum      , frame->tcp.rst      ); 
   printf ("                          ip.src_ip   = %-8llx   tcp.syn          = %-8llx\n" , frame->ip.src_ip        , frame->tcp.syn      ); 
   printf ("                          ip.dest_ip  = %-8llx   tcp.fin          = %-8llx\n" , frame->ip.dest_ip       , frame->tcp.fin      ); 
   printf ("                                                   tcp.win_size     = %-8llx\n" , frame->tcp.win_size                         ); 
   printf ("                                                   tcp.checksum     = %-8llx\n" , frame->tcp.checksum                         );
   printf ("                                                   tcp.urg_ptr      = %-8llx\n" , frame->tcp.urg_ptr                          );
   printf ("---------------------------------------------------------------------------\n");
}

/*
frame->mac.dest      = 0x112233445566; 
frame->mac.src       = 0xaabbccddeeff; 
frame->mac.type      = 0x0; 
frame->ip.ver        = 0x0; 
frame->ip.hlen       = 0x0; 
frame->ip.tos        = 0x0; 
frame->ip.len        = 0x0; 
frame->ip.id         = 0x0; 
frame->ip.flags      = 0x0; 
frame->ip.offset     = 0x0; 
frame->ip.ttl        = 0x0; 
frame->ip.proto      = 0x0; 
frame->ip.checksum   = 0x0; 
frame->ip.src_ip     = 0x11223344; 
frame->ip.dest_ip    = 0x0; 
frame->tcp.src_port  = 0x0; 
frame->tcp.dest_port = 0x0; 
frame->tcp.seq_num   = 0x0;
frame->tcp.ack_num   = 0x0; 
frame->tcp.hlen      = 0x0; 
frame->tcp.reserved  = 0x0; 
frame->tcp.urg       = 0x0; 
frame->tcp.ack       = 0x0; 
frame->tcp.psh       = 0x0; 
frame->tcp.rst       = 0x0; 
frame->tcp.syn       = 0x0; 
frame->tcp.fin       = 0x0; 
frame->tcp.win_size  = 0x0; 
frame->tcp.checksum  = 0x0; 
frame->tcp.urg_ptr   = 0x0; 

int32_t __builtin_bswap32 (int32_t x)
int64_t __builtin_bswap64 (int64_t x)

*/
