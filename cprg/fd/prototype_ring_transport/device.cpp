#include "common.h"


class device
{
public:
   device();
   ~device();
   void connect_to_server();
   void map_shmem();
   void start_tx_thread();
   void join_threads();

private:
   void tx_thread();
   void get_shmem();
   void map_shmem_to_vspace();      
   void reset();

private:
   struct sockaddr_un server;
   int client_fd;
   int shmem;
   char * shmem_ptr;
   virtqueue *vq;
   unsigned int vq_size;
   thread tx_thr;
   bool tx_thread_started;

private:
   unsigned int last_avail_idx;
   unsigned int last_used_idx;
};


void device::join_threads()
{
   printf ("Joining threads\n");
   tx_thr.join();
}


void device::start_tx_thread()
{
   printf ("Starting Tx thread\n");
   tx_thread_started = true;
   tx_thr = thread (&device::tx_thread, this);
   pthread_setname_np(tx_thr.native_handle(), "DevTxThread");
}


void device::tx_thread()
{
   uint32_t idx=0;
   uint32_t data=0;
   uint32_t len=0;

   while (1)
   {
      if (last_avail_idx != vq->avail.idx)
      {
         printf ("Reading a frame..index=%d data=%x\n",idx, data);
         // memset (vq->guest_mem[idx].mem, data, PAGE_SIZE);
         // // updated avail ring {
         // vq->used.ring[vq->used.idx].id = idx;
         // vq->used.ring[vq->used.idx].len = len;
         // vq->used.idx++;
         // // }
         // idx++;
         // data++;
         // len += 10;
         last_avail_idx++;
      }
      sleep (1);
   }
}


device::device()
{
   reset();
}


void device::reset()
{
   client_fd = -1;
   shmem = -1;
   vq_size = 0;
   last_avail_idx = 0;
   last_used_idx = 0;
   tx_thread_started = false;
}


void device::map_shmem()
{
   get_shmem();
   map_shmem_to_vspace();
}


void device::get_shmem()
{
   shmem = recv_fd (client_fd);
   unsigned int avail= ioctl(shmem, FIONREAD, &avail);
   printf ("Received shmem fd = %d size=%d\n", shmem, avail);
}


void device::map_shmem_to_vspace()
{
   vq_size = sizeof (struct virtqueue);
   shmem_ptr = (char*) mmap (0, vq_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem, 0);
   vq = (virtqueue*) shmem_ptr;

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
      << hex << showbase << setw(15) << vq->desc[i].addr
      << dec << setw(10) << vq->desc[i].len 
      << setw(10) << vq->desc[i].flags
      << setw(10) << vq->desc[i].next << endl;
   }
   // pring a page
   for (uint16_t i=0; i<PAGE_SIZE; i++) 
   {
      if (i%25==0) cout << endl;
      printf ("%-2c", vq->guest_mem[0].mem[i]);
   }
   cout << endl;
}


device::~device()
{
   munmap (shmem_ptr, vq_size);
}


void device::connect_to_server()
{
   client_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   server.sun_family = AF_UNIX;
   strcpy(server.sun_path, "/tmp/seqpacket.sock");
   if (connect(client_fd, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
   {
      perror("Connecting stream socket");
      close(client_fd);
      exit(1);
   }
   printf ("Connection accepted\n");
}


int main ()
{
   device device;
   device.connect_to_server();
   device.map_shmem();
   device.start_tx_thread();
   device.join_threads();
   return 0;
}
