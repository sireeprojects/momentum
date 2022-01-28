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

#define DMSG(...)                \
   printf ("\n[Data] " __VA_ARGS__); \
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

void set_data_rnd (ring_info *r)
{
   for (int i = 0; i<2; i++)
      {
      r->vq[i].size           = rand();
      r->vq[i].last_avail_idx = rand();
      r->vq[i].last_used_idx  = rand();
      r->vq[i].call_fd        = rand();
      r->vq[i].kick_fd        = rand();
      r->vq[i].enable         = rand();
      }

   uint64_t tmp;

   for (int i = 0; i<8; i++)
      {
      tmp = rand(); r->reg[i].gpa           = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].size          = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].qva           = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].mmap_offset   = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].mmap_addr     = (tmp << 32) | rand();
      }
}

void print_data (ring_info *r)
{
   for (int i = 0; i<2; i++)
      {
      DMSG ("r->vq[%d].size           = 0x%lx", i, r->vq[i].size           );
      DMSG ("r->vq[%d].last_avail_idx = 0x%lx", i, r->vq[i].last_avail_idx );
      DMSG ("r->vq[%d].last_used_idx  = 0x%lx", i, r->vq[i].last_used_idx  );
      DMSG ("r->vq[%d].call_fd        = 0x%lx", i, r->vq[i].call_fd        );
      DMSG ("r->vq[%d].kick_fd        = 0x%lx", i, r->vq[i].kick_fd        );
      DMSG ("r->vq[%d].enable         = 0x%lx", i, r->vq[i].enable         );
      }
   
   for (int i = 0; i<8; i++)
      {
      DMSG ("r->reg[%d].gpa           = 0x%lx", i, r->reg[i].gpa         );
      DMSG ("r->reg[%d].size          = 0x%lx", i, r->reg[i].size        );
      DMSG ("r->reg[%d].qva           = 0x%lx", i, r->reg[i].qva         );
      DMSG ("r->reg[%d].mmap_offset   = 0x%lx", i, r->reg[i].mmap_offset );
      DMSG ("r->reg[%d].mmap_addr     = 0x%lx", i, r->reg[i].mmap_addr   );
      }
}
