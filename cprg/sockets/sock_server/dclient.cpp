#include "common.h"

#define SOCKNAME "/tmp/avip_data_path.sock"

int main ()
{
   tx_buf = new unsigned char [MAX_FRAME_SIZE];

   control_msg cm;
   char buf[256];
   bzero(&buf, 256);
   struct sockaddr_un server;

   // create socket
   int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, SOCKNAME);
   
   // request a connection from server
   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
       close(client_fd);
       CMSG("Socket Error");
       exit(1);
   }

   // wait for sometime before any operation
   sleep(5);

   // request rng information
   cm.type = 5;
   cm.reserved = 0xaabbccdd11223344;
   send (client_fd, (char*)&cm, 12, 0);

   // read response from adaptor and print ring information
   int nbytes = recv (client_fd, (char*)&ri, sizeof ri, 0);
   print_data (&ri);

   // calculate mmap_addr
   for (int i=0; i<ri.nregions; i++)
   {
      void *mmap_addr;
      CMSG ("fds = %d", ri.reg[i].fds);
      mmap_addr = mmap (
            0,
            ri.reg[i].size + ri.reg[i].mmap_offset,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            ri.reg[i].fds,
            0
            );
      if (mmap_addr == MAP_FAILED)
      {
         perror ("\nmap failed: ");
         exit(1);
      }
      ri.reg[i].mmap_addr = (uint64_t)(uintptr_t)mmap_addr;
   }
  
   // find descriptor address 
   for (int i=0; i<2; i++)
   {
      vq[i].desc = (struct vring_desc *)(uintptr_t)qva_to_va  (ri.vq[i].desc_user_addr);
      vq[i].used = (struct vring_used *)(uintptr_t)qva_to_va  (ri.vq[i].used_user_addr);
      vq[i].avail = (struct vring_avail *)(uintptr_t)qva_to_va(ri.vq[i].avail_user_addr);
   }

   // attempt to read frames from the unaltered adaptor information
   CMSG("TX Thread Started");
   while (1) {
      process_frames_from_ixia();
      sleep(1);
   }

   return 0;
}
