#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <iomanip>


using namespace std;


string readable_fs (double size)
{
    int i = 0;
    char buf[100];
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1023) {
      size /= 1024;
      i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}


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
   struct vring_used_elem ring[RING_SIZE];
};


struct vring_avail
{
   uint16_t flags;
   uint16_t idx;
   uint16_t ring[RING_SIZE];
};


struct page 
{
   char mem [PAGE_SIZE];
};


struct virtqueue
{
   struct vring_desc desc [RING_SIZE];
   struct vring_avail avail;
   struct vring_used used;
   struct page guest_mem[RING_SIZE];
};
