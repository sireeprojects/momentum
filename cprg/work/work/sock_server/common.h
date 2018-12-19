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
#include <assert.h>
#include <sys/mman.h>

#define SMSG(...)                \
   printf ("\n[Server] " __VA_ARGS__); \
   fflush(stdout);

#define CMSG(...)                \
   printf ("\n[Client] " __VA_ARGS__); \
   fflush(stdout);

#define DMSG(...)                \
   printf ("\n[Data] " __VA_ARGS__); \
   fflush(stdout);

#define MAX_FRAME_SIZE     10240
#define VRING_DESC_F_WRITE         2
#define VRING_DESC_F_NEXT          1

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
   uint32_t index           : 32 ;
   uint32_t flags           : 32 ;
   uint64_t desc_user_addr  : 64 ;
   uint64_t used_user_addr  : 64 ;
   uint64_t avail_user_addr : 64 ;
   }__attribute__((__packed__));

struct region_info 
   {
   uint64_t gpa             : 64 ;
   uint64_t size            : 64 ;
   uint64_t qva             : 64 ;
   uint64_t mmap_offset     : 64 ;
   int fds                  : 32 ;
   uint64_t mmap_addr       : 64 ;
   }__attribute__((__packed__));


struct ring_info
{
   struct vq_info vq[2];
   uint32_t nregions  : 32;
   struct region_info reg[8];
}__attribute__((__packed__));


void set_data (ring_info *r)
{
   for (int i = 0; i<2; i++)
      {
      r->vq[i].size            = i;
      r->vq[i].last_avail_idx  = i;
      r->vq[i].last_used_idx   = i;
      r->vq[i].call_fd         = i;
      r->vq[i].kick_fd         = i;
      r->vq[i].enable          = i;
      r->vq[i].index           = i;
      r->vq[i].flags           = i;
      r->vq[i].desc_user_addr  = i;
      r->vq[i].used_user_addr  = i;
      r->vq[i].avail_user_addr = i;
      }
   r->nregions = 0;
   for (int i = 0; i<8; i++)
      {
      r->reg[i].gpa           = i;
      r->reg[i].size          = i;
      r->reg[i].qva           = i;
      r->reg[i].mmap_offset   = i;
      r->reg[i].fds           = i;
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
      r->vq[i].index           = rand();
      r->vq[i].flags           = rand();
      r->vq[i].desc_user_addr  = rand();
      r->vq[i].used_user_addr  = rand();
      r->vq[i].avail_user_addr = rand();
      }
   r->nregions = rand();
   uint64_t tmp;
   for (int i = 0; i<8; i++)
      {
      tmp = rand(); r->reg[i].gpa           = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].size          = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].qva           = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].mmap_offset   = (tmp << 32) | rand();
      tmp = rand(); r->reg[i].fds           = (tmp << 32) | rand();
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
      //
      DMSG ("r->vq[%d].index           = 0x%lx", i, r->vq[i].index           );
      DMSG ("r->vq[%d].flags           = 0x%lx", i, r->vq[i].flags           );
      DMSG ("r->vq[%d].desc_user_addr  = 0x%lx", i, r->vq[i].desc_user_addr  );
      DMSG ("r->vq[%d].used_user_addr  = 0x%lx", i, r->vq[i].used_user_addr  );
      DMSG ("r->vq[%d].avail_user_addr = 0x%lx", i, r->vq[i].avail_user_addr );
      }
   DMSG ("r->nregions                 = 0x%lx", r->nregions             );
   for (int i = 0; i<8; i++)
      {
      DMSG ("r->reg[%d].gpa           = 0x%lx", i, r->reg[i].gpa         );
      DMSG ("r->reg[%d].size          = 0x%lx", i, r->reg[i].size        );
      DMSG ("r->reg[%d].qva           = 0x%lx", i, r->reg[i].qva         );
      DMSG ("r->reg[%d].mmap_offset   = 0x%lx", i, r->reg[i].mmap_offset );
      DMSG ("r->reg[%d].fds           = 0x%lx", i, r->reg[i].fds         );
      DMSG ("r->reg[%d].mmap_addr     = 0x%lx", i, r->reg[i].mmap_addr   );
      }
}

unsigned char *tx_buf;

ring_info ri;

struct virtqueue 
   {
   vring_avail *avail;
   vring_desc *desc;
   vring_used *used;
   };

virtqueue vq[2];

uint64_t qva_to_va (uint64_t qemu_addr)
{
   uint32_t i;
   for (i = 0; i < ri.nregions; i++)
   {
      region_info *r = &ri.reg[i];
      if ((qemu_addr >= r->qva) && (qemu_addr < (r->qva + r->size)))
         return qemu_addr - r->qva + r->mmap_addr + r->mmap_offset;
   }
   assert(!"address not found in regions\n");
   return 0;
}


uint64_t gpa_to_va (uint64_t guest_addr, int mode)
{
   uint32_t i;
   for (i = 0; i < ri.nregions; i++)
   {
      region_info *r = &ri.reg[i];
      if ((guest_addr >= r->gpa) && (guest_addr < (r->gpa + r->size)))
         return guest_addr - r->gpa + r->mmap_addr + r->mmap_offset;
   }
   if (mode==1) assert(!"Transmit side address not found in regions\n");
   if (mode==0) assert(!"Receive side address not found in regions\n");

   return 0;
}

void print_frame (uint8_t *buf, uint32_t len)
{
   printf("\n---[TX Frame len=%d]---------------------------------------------- ", len);
   for (int i=0; i<len; i++) {
      if (i % 16 == 0) printf("\n");
      printf ("%02x ", buf[i]);
   }
   printf("\n----------------------------------------------------------- ");
}


int read_frames_from_ixia()
{
   struct vring_desc *desc   = vq[1].desc;
   struct vring_avail *avail = vq[1].avail;
   struct vring_used *used   = vq[1].used;

   unsigned int size = ri.vq[1].size;
   uint16_t a_index =  ri.vq[1].last_avail_idx % size;
   uint16_t u_index =  ri.vq[1].last_used_idx % size;
   uint16_t d_index = avail->ring[a_index];

   uint32_t i, len = 0;
   size_t buf_size = MAX_FRAME_SIZE;

   i = d_index;
   do {
      void *chunk_start = (void *)(uintptr_t)gpa_to_va(desc[i].addr, 1);
      uint32_t chunk_len = desc[i].len;

      assert(!(desc[i].flags & VRING_DESC_F_WRITE));

      if (len + chunk_len < buf_size) {
         memcpy((tx_buf) + len, chunk_start, chunk_len);
      }
      else {
         CMSG ("Error: too long packet. Dropping...");
         break;
      }
      len += chunk_len;

      if (!(desc[i].flags & VRING_DESC_F_NEXT))
         break;

      i = desc[i].next;
   } while (1);

   if (!len) return -1;

   used->ring[u_index].id = d_index;
   used->ring[u_index].len = len;

   // print frame content
   print_frame (tx_buf, len);
}

void process_frames_from_ixia()
{
   CMSG ("step 1");
   struct vring_avail *avail = vq[1].avail;
   CMSG ("step 2");
   struct vring_used *used =   vq[1].used;
   CMSG ("step 2a");
   if (ri.vq[1].last_avail_idx != avail->idx)
   {
      CMSG ("step 3");
      read_frames_from_ixia();
      CMSG ("step 31");
      ri.vq[1].last_avail_idx++;
      CMSG ("step 32");
      ri.vq[1].last_used_idx++;
   }   
   used->idx = ri.vq[1].last_used_idx;
}
