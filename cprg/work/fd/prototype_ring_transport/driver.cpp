#include "common.h"


class driver
{
public:
   driver();
   ~driver();
   void create_shared_memory();
   void initialize_socket();
   void start_sock_thread();
   void start_tx_thread();
   void reset();
   void join_threads();

private:
   void sock_thread();
   void tx_thread();
   void add_fd (int fd);
   void init_vq();

private:
   fd_set fds;
   socklen_t len;
   virtqueue *vq;
   struct sockaddr_un server_addr;
   int max_fd;
   unsigned int timeout;
   unsigned int vq_size;
   int server_fd;
   int client_fd;
   int shmem;
   char *shmem_ptr;
   bool tx_thread_started;
   bool sock_thread_started;
   thread tx_thr;
   thread sock_thr;

private:
   unsigned int last_avail_idx;
};


driver::driver()
{
   printf ("Resetting driver\n");
   reset();
}


driver::~driver()
{
   printf ("Deleting Shared memory\n");
   shmctl(shmem, IPC_RMID, NULL);
}


void driver::reset()
{
   timeout = 200000;
   server_fd = -1;
   client_fd = -1;
   shmem = -1;
   max_fd = -1;
   vq_size = sizeof (struct virtqueue);
   tx_thread_started = false;
   sock_thread_started = false;
   last_avail_idx = 0;
}


void driver::create_shared_memory()
{
   shmem = shm_open("shmem_driver", O_CREAT | O_RDWR, 0777);
   ftruncate (shmem, vq_size);
   shmem_ptr = (char*) mmap (0, vq_size, PROT_WRITE, MAP_SHARED, shmem, 0);
   printf ("Creating Shared memory, fd = %d size = %d\n", shmem, vq_size);
   vq = (virtqueue*) shmem_ptr;
   init_vq(); 
}

void driver::init_vq()
{
   printf ("Initializign VirtQueue\n");
   // init desc ring
   for (uint16_t i=0; i<RING_SIZE; i++) 
   {
      vq->desc[i].len = 4096; // will be updated by the driver after writing
      vq->desc[i].flags = VRING_DESC_F_WRITE;
      vq->desc[i].next = i+1;
      vq->desc[i].addr = i;
      if (i==(RING_SIZE-1)) 
         vq->desc[i].next = 0;
   }

   memset (vq->guest_mem[0].mem, 0, PAGE_SIZE);

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
      printf ("%-02x ", vq->guest_mem[0].mem[i]);
   }
   cout << endl;
}


void driver::initialize_socket()
{
   printf ("Creating Socket\n");
   // create socket
   server_fd = socket (AF_UNIX, SOCK_SEQPACKET, 0);
   if (server_fd==-1) perror ("socket failed");
   add_fd (server_fd);

   // bind to local address
   len = sizeof(struct sockaddr_un);
   server_addr.sun_family = AF_UNIX;
   strncpy (server_addr.sun_path, "/tmp/seqpacket.sock", (len - sizeof(short)));
   unlink (server_addr.sun_path);
   int success = bind (server_fd, (struct sockaddr*) &server_addr, len);
   if (success==-1) perror ("bind failed");

   // start listening
   listen (server_fd, 1);
   printf ("Waiting for connection request...\n");
}


void driver::start_tx_thread()
{
   printf ("Starting Tx thread\n");
   tx_thread_started = true;
   tx_thr = thread (&driver::tx_thread, this);
   pthread_setname_np(tx_thr.native_handle(), "DrvTxThread");
}


void driver::start_sock_thread()
{
   printf ("Starting Socket thread\n");
   sock_thread_started = true;
   sock_thr = thread (&driver::sock_thread, this);
   pthread_setname_np(sock_thr.native_handle(), "DrvSockThread");
}


void driver::sock_thread()
{
   while (1)
   {
      struct timeval tv;
      tv.tv_sec = timeout / 1000000;
      tv.tv_usec = timeout % 1000000;
      tv.tv_sec = 1;
      tv.tv_usec = 1;
      fd_set fds_rd = fds;
      fd_set fds_wr = fds;
      fd_set fds_ex = fds;
      int rc = select(max_fd+1, &fds_rd, 0, 0, &tv);

      int fd;
      for (fd=0; fd<max_fd+1; fd++)
      {
         if (fd==server_fd && FD_ISSET(fd, &fds_rd)) {
            struct sockaddr_un un;
            socklen_t len = sizeof(un);
            printf ("Connection request received from client\n");
            client_fd = accept (server_fd, (struct sockaddr *) &un, &len);
            printf ("Connection request from client accepted\n");
            // add_fd (client_fd);
            send_fd (client_fd, shmem);
            printf ("Sending Shared memory fd=%d\n", shmem);
            }
         else if (fd==client_fd && FD_ISSET(fd, &fds_rd)) {
            }
      }
   }
}


void driver::tx_thread()
{
   sleep (3);
   uint32_t idx=0;
   uint32_t data=0;
   uint32_t len=0;

   while (1)
   {
      printf ("Adding a frame..index=%d data=%x\n",idx, data);
      memset (vq->guest_mem[idx].mem, data, PAGE_SIZE);
      vq->desc[idx].len = len;
      vq->desc[idx].next = 0;

      // updated avail ring {
      vq->avail.ring[vq->avail.idx] = idx;
      vq->avail.idx++;
      // }

      idx++;
      data++;
      len += 10;
      sleep (1);
   }
}


void driver::join_threads()
{
   printf ("Joining threads\n");
   sock_thr.join();
   tx_thr.join();
}


void driver::add_fd (int fd)
{
   FD_SET(fd, &fds);
   if (fd > max_fd) max_fd = fd;
   printf ("Socket Added (sock_id=%d max=%d)\n", fd, max_fd);
}


int main ()
{
   driver driver;
   driver.create_shared_memory();
   driver.initialize_socket();
   driver.start_sock_thread();
   driver.start_tx_thread();
   driver.join_threads();
   return 0;
}
