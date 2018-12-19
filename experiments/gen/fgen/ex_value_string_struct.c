#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


enum fields {
   mac_pre     = 0,
   mac_dest    = 1,
   mac_src     = 2,
   mac_lt      = 3,
   ip4_ver     = 4, 
   ip4_hlen    = 5,
   ip4_tos     = 6,
   ip4_len     = 7,
   ip4_id      = 8,
   ip4_flags   = 9,
   ip4_offset  = 10,    
   ip4_ttl     = 11,
   ip4_proto   = 12,
   ip4_csum    = 13,
   ip4_sip     = 14,
   ip4_dip     = 15,
   tcp_sport   = 16,
   tcp_dport   = 17,
   tcp_seqnum  = 18,
   tcp_acknum  = 19,
   tcp_hlen    = 20,
   tcp_res     = 21,
   tcp_urg     = 22,
   tcp_ack     = 23,
   tcp_psh     = 24,
   tcp_rst     = 25,
   tcp_syn     = 26,
   tcp_fin     = 27,
   tcp_winsize = 28,
   tcp_csum    = 29,
   tcp_urgptr  = 30,
};                  


uint32_t total_fields = 31;


enum sizetype {
   bytes,
   bits
};


struct fieldinfo {
   char name [32];
   uint16_t size;
   enum sizetype sizetype;
   uint64_t val;
};


struct fieldinfo attr[]={
   { "MAC_Preamble            " ,  8 , bytes  , 0 },
   { "MAC_Destination Address " ,  6 , bytes  , 0 },
   { "MAC_Source Address      " ,  6 , bytes  , 0 },
   { "MAC_Length/Type         " ,  2 , bytes  , 0 },
   { "IPv4_ver                " ,  4 , bits   , 0 },
   { "IPv4_hlen               " ,  4 , bits   , 0 },
   { "IPv4_tos                " ,  1 , bytes  , 0 },
   { "IPv4_len                " ,  2 , bytes  , 0 },
   { "IPv4_id                 " ,  2 , bytes  , 0 },
   { "IPv4_flags              " ,  3 , bits   , 0 },
   { "IPv4_offset             " , 13 , bits   , 0 },
   { "IPv4_ttl                " ,  1 , bytes  , 0 },
   { "IPv4_proto              " ,  1 , bytes  , 0 },
   { "IPv4_checksum           " ,  2 , bytes  , 0 },
   { "IPv4_src_ip             " ,  4 , bytes  , 0 },
   { "IPv4_dest_ip            " ,  4 , bytes  , 0 },
   { "TCP_src_port            " ,  2 , bytes  , 0 },
   { "TCP_dest_port           " ,  2 , bytes  , 0 },
   { "TCP_seq_num             " ,  4 , bytes  , 0 },
   { "TCP_ack_num             " ,  4 , bytes  , 0 },
   { "TCP_hlen                " ,  4 , bits   , 0 },
   { "TCP_reserved            " ,  6 , bits   , 0 },
   { "TCP_urg                 " ,  1 , bits   , 0 },
   { "TCP_ack                 " ,  1 , bits   , 0 },
   { "TCP_psh                 " ,  1 , bits   , 0 },
   { "TCP_rst                 " ,  1 , bits   , 0 },
   { "TCP_syn                 " ,  1 , bits   , 0 },
   { "TCP_fin                 " ,  1 , bits   , 0 },
   { "TCP_win_size            " ,  2 , bytes  , 0 },
   { "TCP_checksum            " ,  2 , bytes  , 0 },
   { "TCP_urg_ptr             " ,  2 , bytes  , 0 }
};


uint32_t pack_seq [] = { 30,
     1,  2,  3,  4,  5,  6,  7,  8,  9, 
    10, 11, 12, 13, 14, 15, 16, 17, 18, 
    19, 20, 21, 22, 23, 24, 25, 26, 27, 
    28, 29, 30 
};


void memcpyb(uint8_t *dest, uint8_t *src, size_t size)
{
   size_t n=0;
   for (n=0; n<size; n++) dest[n] = src[size-n-1];
}


uint32_t pack(struct fieldinfo *f, uint32_t seq[], uint8_t *packed)
{
   uint32_t n=0;
   uint32_t nbytes=0;
   uint32_t nbits=0;
   uint32_t offset = 0;
   uint64_t bitbuf=0;
   uint32_t bitbuf_start=0;

   while (n < seq[0]) 
   {
      switch (f[seq[n+1]].sizetype) 
      {
         case bytes:{
            nbytes += f[seq[n+1]].size;
            memcpyb((packed+offset), (uint8_t*)&f[seq[n+1]].val, f[seq[n+1]].size);
            offset = offset + f[seq[n+1]].size;
            n++;
            break;
            }

         case bits:{
            bitbuf_start=1;
            bitbuf=0;
            nbits=0;

            while (f[seq[n+1]].sizetype==bits) 
            {
               if (bitbuf_start) 
               {
                  bitbuf = bitbuf | f[seq[n+1]].val;
                  bitbuf_start=0;
               }
               else 
               {
                  bitbuf = (bitbuf<<f[seq[n+1]].size) | f[seq[n+1]].val;
               }
               nbits += f[seq[n+1]].size;
               n++;
            }
            nbytes += (nbits/8);
            memcpyb((packed+offset), (uint8_t*) &bitbuf, nbits/8);
            offset += nbits/8;
            break;
            }
      }
   }
   return nbytes;
}


void set_field(struct fieldinfo *f, enum fields id, uint64_t value) 
{
   f[id].val = value;
}


void print_fields(uint32_t seq[], struct fieldinfo *f)
{
   uint32_t n=0;
   for (n=0; n<seq[0]; n++) 
   {
      printf ("%s= 0x%llx\n", 
         f[seq[n+1]].name,
         f[seq[n+1]].val);
   }
}


void print_raw(uint8_t*data, uint32_t len)
{
   uint32_t x;
   printf ("---------------\n");
   printf ("Packed Data:%d \n", len);
   printf ("---------------\n");
   for (x=0; x<len; x++) {
      printf ("%02x ", data[x]);
      if (x%8==7) printf ("  ");
      if (x%16==15) printf ("\n");
   }
   printf ("\n");
}


struct fieldinfo *create_new()
{
   struct fieldinfo *txn;
   txn = (struct fieldinfo*) malloc (sizeof(attr));
   memcpy (txn, &attr, sizeof (attr));
   return txn;
}


void reset_all_fleilds(struct fieldinfo *f)
{
   uint32_t n=0;
   for (n=0; n<total_fields; n++) f[n].val=0;
}


int main ()
{
   struct fieldinfo *txn = create_new();

   set_field (txn, mac_pre     , 0xa5b5c5d5e5f51525 );
   set_field (txn, mac_dest    , 0x08002738dbed     );
   set_field (txn, mac_src     , 0x080027973f45     );
   set_field (txn, mac_lt      , 0x0800             );
   set_field (txn, ip4_ver     , 0x4                );
   set_field (txn, ip4_hlen    , 0x5                );
   set_field (txn, ip4_tos     , 0x0                );
   set_field (txn, ip4_len     , 0x0046             );
   set_field (txn, ip4_id      , 0x00ec             );
   set_field (txn, ip4_flags   , 0x02               );
   set_field (txn, ip4_offset  , 0x0                );   
   set_field (txn, ip4_ttl     , 0x80               );
   set_field (txn, ip4_proto   , 0x06               );
   set_field (txn, ip4_sip     , 0x01010102         );
   set_field (txn, ip4_dip     , 0x01010101         );
   set_field (txn, tcp_sport   , 1042               );
   set_field (txn, tcp_dport   , 53                 );
   set_field (txn, tcp_seqnum  , 0xd1f8c117         );
   set_field (txn, tcp_acknum  , 0x5ff5a8bd         );
   set_field (txn, tcp_hlen    , 5                  );
   set_field (txn, tcp_res     , 0x00               );
   set_field (txn, tcp_urg     , 0x00               );
   set_field (txn, tcp_ack     , 0x00               );
   set_field (txn, tcp_psh     , 0x00               );
   set_field (txn, tcp_rst     , 0x00               );
   set_field (txn, tcp_syn     , 0x00               );
   set_field (txn, tcp_fin     , 0x00               );
   set_field (txn, tcp_winsize , 64400              );
   set_field (txn, tcp_csum    , 0x00               );
   set_field (txn, tcp_urgptr  , 0x00               );

   uint8_t *bytestream = (uint8_t*) malloc (1024);
   uint32_t nbytes = pack(txn, pack_seq, bytestream);

   print_fields(pack_seq, txn);
   print_raw(bytestream,nbytes);

   reset_all_fleilds (txn);
   print_fields(pack_seq, txn);

   return 0;
}
