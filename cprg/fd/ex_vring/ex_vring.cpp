#define RING_SIZE 10
#define PAGE_SIZE 4096
#define OFFSET(x, y)  &((x *)0)->y

#include "common.h"

int main ()
{
   virtqueue *smem = new virtqueue;

   cout <<left<<setw(30) << "Size of Vqueue in Bytes"   <<left<<setw(2) << "=" << dec << sizeof (struct virtqueue) << endl;
   cout <<left<<setw(30) << "Size of Vqueue (Readable)" <<left<<setw(2) << "=" << readable_fs (sizeof (struct virtqueue))<< endl;
   cout <<left<<setw(30) << "Addr of Vqueue"            <<left<<setw(2) << "=" << smem             << endl;
   cout <<left<<setw(30) << "Addr of Desc"              <<left<<setw(2) << "=" << &smem->desc      << endl;
   cout <<left<<setw(30) << "Addr of Avail"             <<left<<setw(2) << "=" << &smem->avail     << endl;
   cout <<left<<setw(30) << "Addr of Used"              <<left<<setw(2) << "=" << &smem->used      << endl;
   cout <<left<<setw(30) << "Addr of Guest mem"         <<left<<setw(2) << "=" << &smem->guest_mem << endl;

   uint64_t addr_of_desc      = (uintptr_t) &smem->desc;
   uint64_t addr_of_avail     = (uintptr_t) &smem->avail;
   uint64_t addr_of_used      = (uintptr_t) &smem->used;
   uint64_t addr_of_gpa_first = (uintptr_t) &smem->guest_mem;
   uint64_t addr_of_gpa_last  = (uintptr_t) &smem->guest_mem[RING_SIZE];

   cout << endl;
   cout <<left<<setw(30) << "Value of Desc"      <<left<<setw(2) <<hex<<showbase << "=" << addr_of_desc      << endl;
   cout <<left<<setw(30) << "Value of Avail"     <<left<<setw(2) <<hex<<showbase << "=" << addr_of_avail     << endl;
   cout <<left<<setw(30) << "Value of Used"      <<left<<setw(2) <<hex<<showbase << "=" << addr_of_used      << endl;
   // cout <<left<<setw(30) << "Value of Guest Fir" <<left<<setw(2) <<hex<<showbase << "=" << addr_of_gpa_first << endl;
   cout <<left<<setw(30) << "Value of Guest Sec" <<left<<setw(2) <<hex<<showbase << "=" << addr_of_gpa_last  << endl;

   cout << endl;
   cout <<left<<setw(30) << "Size of Vqueue"    <<left<<setw(2) << "=" << dec << readable_fs (sizeof (struct virtqueue))     << endl;
   cout <<left<<setw(30) << "Size of Desc"      <<left<<setw(2) << "=" << readable_fs ((addr_of_avail-addr_of_desc))         << endl;
   cout <<left<<setw(30) << "Size of Avail"     <<left<<setw(2) << "=" << readable_fs ((addr_of_used-addr_of_avail))         << endl;
   cout <<left<<setw(30) << "Size of Used"      <<left<<setw(2) << "=" << readable_fs ((addr_of_gpa_first-addr_of_used))     << endl;
   cout <<left<<setw(30) << "Size of Guest mem" <<left<<setw(2) << "=" << readable_fs ((addr_of_gpa_last-addr_of_gpa_first)) << endl;

   cout << endl;
   for (uint16_t i=0; i<RING_SIZE; i++) 
   {
      cout <<left<<setw(30) << "Addr of Guest mem"         <<left<<setw(2) << "=" << &smem->guest_mem[i] << endl;
   }

   // init desc ring
   for (uint16_t i=0; i<RING_SIZE; i++) 
   {
      smem->desc[i].len = 4096; // will be updated by the driver after writing
      smem->desc[i].flags = 1;
      smem->desc[i].next = i+1;
      smem->desc[i].addr = (uintptr_t) &smem->guest_mem[i];
      if (i==(RING_SIZE-1)) 
         smem->desc[i].next = 0;
   }
   // display desc table
   cout << endl << left \
   << setw(10) << "Idx"
   << setw(15) << "Addr"
   << setw(10) << "Len"
   << setw(10) << "Flags"
   << setw(10) << "Next" << endl;
   for (uint16_t i=0; i<RING_SIZE; i++) 
   {
      cout << left \
      << setw(10) << i
      << hex << showbase << setw(15) << smem->desc[i].addr
      << dec << setw(10) << smem->desc[i].len 
      << setw(10) << smem->desc[i].flags
      << setw(10) << smem->desc[i].next << endl;
   }


   // // check if gust mem is in expected offset
   // for (uint16_t i=0; i<RING_SIZE; i++) {
   //    printf ("Guest Mem Offset = \t%d \t0x%llx\n", 
   //          OFFSET(struct virtqueue, guest_mem[i]), 
   //          smem->guest_mem[i].mem
   //          );
   //    }


   // // initialize avail ring
   // smem->avail.flags = 0; // not used
   // smem->avail.idx = 0;
   // for (uint16_t i=0; i<RING_SIZE; i++){
   //    smem->avail.ring [i] = i;
   //    }
   // // print Avail table
   // printf ("Flag=%d \tIdx=%d \tavail.ring\n", smem->avail.flags,  smem->avail.idx);
   // for (uint16_t i=0; i<RING_SIZE; i++) {
   //    printf ("\t\t%d\n", 
   //       smem->avail.ring[i]
   //    );
   //    }


   // // init used ring
   // smem->used.flags = 0; // not used
   // smem->used.idx = 0;
   // for (uint16_t i=0; i<RING_SIZE; i++){
   //    smem->used.ring[i].id = 0;
   //    smem->used.ring[i].len = 0;
   //    }
   // // print used table
   // printf ("Flag=%d \tIdx=%d \tused_ring.id \tused_ring.len\n", smem->used.flags,  smem->used.idx);
   // for (uint16_t i=0; i<RING_SIZE; i++) {
   //    printf ("\t\t%d\t\t%d\n", 
   //       smem->used.ring[i].id,
   //       smem->used.ring[i].len
   //    );
   //    }

   delete smem;
   return 0;
}
