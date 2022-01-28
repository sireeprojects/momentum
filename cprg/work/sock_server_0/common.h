#include <iostream>
#include <sys/select.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SMSG(...)                \
   printf ("\n[Server] " __VA_ARGS__); \
   fflush(stdout);

#define CMSG(...)                \
   printf ("\n[Client] " __VA_ARGS__); \
   fflush(stdout);


struct control_msg 
{
   uint32_t type     : 32;
   uint64_t reserved : 64;
}__attribute__((__packed__));


struct vring_avail
   {
   uint16_t flags;
   uint16_t idx;
   uint16_t ring[0];
   };

struct vring_desc
   {
   uint64_t addr;
   uint32_t len;
   uint16_t flags;
   uint16_t next;
   };

struct vring_used_elem
   {
   uint32_t id;
   uint32_t len;
   };

struct vring_used
   {
   uint16_t flags;
   uint16_t idx;
   struct vring_used_elem ring[0];
   };

struct vh_virtqueue
   {
   struct vring_desc  *desc;
   struct vring_avail *avail;
   struct vring_used  *used;
   uint32_t           size;
   uint16_t           last_avail_idx;
   uint16_t           last_used_idx;
   int                call_fd;
   int                kick_fd;
   int                enable;
   };

struct vh_device_region
   {
   uint64_t gpa;
   uint64_t size;
   uint64_t qva;
   uint64_t mmap_offset;
   uint64_t mmap_addr;
   };


struct vq_info  // size = 160
   {
   uint32_t size            : 32 ;
   uint16_t last_avail_idx  : 16 ;
   uint16_t last_used_idx   : 16 ;
   int      call_fd         : 32 ;
   int      kick_fd         : 32 ;
   int      enable          : 32 ;
   }__attribute__((__packed__));

struct region_info 
   {
   uint64_t gpa             : 64 ;
   uint64_t size            : 64 ;
   uint64_t qva             : 64 ;
   uint64_t mmap_offset     : 64 ;
   uint64_t mmap_addr       : 64 ;
   }__attribute__((__packed__));


struct ring_info
{
   struct vq_info vq[2];
   struct region_info reg[8];
}__attribute__((__packed__));


void set_data (ring_info *r)
{
   for (int i = 0; i<2; i++)
      {
      r->vq[i].size           = i;
      r->vq[i].last_avail_idx = i;
      r->vq[i].last_used_idx  = i;
      r->vq[i].call_fd        = i;
      r->vq[i].kick_fd        = i;
      r->vq[i].enable         = i;
      }

   for (int i = 0; i<8; i++)
      {
      r->reg[i].gpa           = i;
      r->reg[i].size          = i;
      r->reg[i].qva           = i;
      r->reg[i].mmap_offset   = i;
      r->reg[i].mmap_addr     = i;
      }
}

void print_data (ring_info *r)
{
   for (int i = 0; i<2; i++)
      {
      printf ("r->vq[%d].size          = %d\n", i, r->vq[i].size           );
      printf ("r->vq[%d].last_avail_idx= %d\n", i, r->vq[i].last_avail_idx );
      printf ("r->vq[%d].last_used_idx = %d\n", i, r->vq[i].last_used_idx  );
      printf ("r->vq[%d].call_fd       = %d\n", i, r->vq[i].call_fd        );
      printf ("r->vq[%d].kick_fd       = %d\n", i, r->vq[i].kick_fd        );
      printf ("r->vq[%d].enable        = %d\n", i, r->vq[i].enable         );
      }
   
   for (int i = 0; i<8; i++)
      {
      printf ("r->reg[%d].gpa         = %d\n", i, r->reg[i].gpa         );
      printf ("r->reg[%d].size        = %d\n", i, r->reg[i].size        );
      printf ("r->reg[%d].qva         = %d\n", i, r->reg[i].qva         );
      printf ("r->reg[%d].mmap_offset = %d\n", i, r->reg[i].mmap_offset );
      printf ("r->reg[%d].mmap_addr   = %d\n", i, r->reg[i].mmap_addr   );
      }
}
