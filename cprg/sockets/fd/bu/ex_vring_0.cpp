#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>

#define RING_SIZE 10
#define PAGE_SIZE 4096
#define OFFSET(x, y)  &((x *)0)->y


using namespace std;

char* readable_fs(double size/*in bytes*/, char *buf) {
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1023) {
      size /= 1024;
      i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

struct vring_desc {
   uint64_t addr;
   uint32_t len;
   uint16_t flags;
   uint16_t next;
   };

struct vring_used_elem {
   uint32_t id;
   uint32_t len;
   };

struct page {
   char mem [PAGE_SIZE];
   };


struct vring 
{
   // desc
   struct vring_desc desc [RING_SIZE]; // 16384
   // avail
   uint16_t avail_flags;
   uint16_t avail_idx;
   uint16_t avail_ring[RING_SIZE]; // 2048
   // used
   uint16_t used_flags;
   uint16_t used_idx;
   struct vring_used_elem used_ring[RING_SIZE]; //8192

   struct page guest_mem[RING_SIZE];
   // unsigned char guest_mem[1048576];
};

int main ()
{
   char buf[100];
   vring *smem = new vring;

   // init desc ring
   for (uint16_t i=0; i<RING_SIZE; i++){
      smem->desc[i].len = 4096;
      smem->desc[i].flags = 0;
      smem->desc[i].next = i+1;
      if (i==(RING_SIZE-1))
         smem->desc[i].next = 0;
      }
   // print desc table
   printf ("Idx \tAddr \tLen \tFlags \tNext\n");
   for (uint16_t i=0; i<RING_SIZE; i++){
      printf ("%2d \t%llu \t%d \t%d \t%d\n", i, 
         smem->desc[i].addr, 
         smem->desc[i].len, 
         smem->desc[i].flags, 
         smem->desc[i].next
      );
      }

   cout << "size = " << dec << sizeof (struct vring) << endl;
   cout << "size = " << readable_fs (sizeof (struct vring), buf)<< endl;
   
   // print address of vring
   printf ("Address of Vring = 0x%x\n", smem);

   // print address of guest memory
   printf ("Address of Guest Mem = 0x%x\n", smem->guest_mem);
   //
   printf ("Guest used_ring = \t0x%llx\n",  &smem->used_ring[RING_SIZE-1].len);

   // check if gust mem is in expected offset
   for (uint16_t i=0; i<RING_SIZE; i++) {
      printf ("Guest Mem Offset = \t%d \t0x%llx\n", 
            OFFSET(struct vring, guest_mem[i]), 
            smem->guest_mem[i].mem
            );
      }

   // initialize avail ring
   smem->avail_flags = 0; // not used
   smem->avail_idx = 0;
   for (uint16_t i=0; i<RING_SIZE; i++){
      smem->avail_ring [i] = i;
      }
   // print Avail table
   printf ("Flag=%d \tIdx=%d \tavail_ring\n", smem->avail_flags,  smem->avail_idx);
   for (uint16_t i=0; i<RING_SIZE; i++) {
      printf ("\t\t%d\n", 
         smem->avail_ring[i]
      );
      }


   // init used ring
   smem->used_flags = 0; // not used
   smem->used_idx = 0;
   for (uint16_t i=0; i<RING_SIZE; i++){
      smem->used_ring[i].id = 0;
      smem->used_ring[i].len = 0;
      }
   // print used table
   printf ("Flag=%d \tIdx=%d \tused_ring.id \tused_ring.len\n", smem->used_flags,  smem->used_idx);
   for (uint16_t i=0; i<RING_SIZE; i++) {
      printf ("\t\t%d\t\t%d\n", 
         smem->used_ring[i].id,
         smem->used_ring[i].len
      );
      }


   delete smem;
   return 0;
}
