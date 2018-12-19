

#include "version.h"


#define typeof(x) __typeof__(x)
#define cat(x,y) x ## y
#define cat2(x,y) cat(x,y)
#define QEMU_BUILD_BUG_ON(x) \
   typedef char cat2(qemu_build_bug_on__,__LINE__)[(x)?-1:1] __attribute__((unused));


#include <iostream>
#include <stddef.h>
#undef __FD_SETSIZE
#define __FD_SETSIZE 4096
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sstream>
#include <vector>
#include <getopt.h>
#include <thread>     
#include <map>        
#include <cmath>      
#include <fcntl.h>
#include "compiler.h" 
#include "atomic.h"   


using namespace std;


// global
string verbosity; 
FILE * log_fp;


// forward declaration
class avip_driver;
class ixia_device;


#define _FILE_OFFSET_BITS  64
#define MAX_FRAME_SIZE     10240
#define METADATA_LENGHT    48
#define FRAME_HDR_LEN_SIZE 4
#define FRAME_HDR_IPG_SIZE 4
#define FRAME_HDR_TS_SIZE  8
#define FRAME_HDR_LAT_SIZE 8


#define IXIA_MSG(...)                          \
   printf ("\n(ixia) " __VA_ARGS__);           \
   fprintf (log_fp, "\n(ixia) " __VA_ARGS__);  \
   fflush(stdout);                             \
   fflush(log_fp);


#define IXIA_INFO(...)                         \
   printf ("\n(info) " __VA_ARGS__);           \
   fprintf (log_fp, "\n(info) " __VA_ARGS__);  \
   fflush(stdout);                             \
   fflush(log_fp);


#define AVIP_MSG(...)                          \
   printf ("\n(avip) " __VA_ARGS__);           \
   fprintf (log_fp, "\n(avip) " __VA_ARGS__);  \
   fflush(stdout);                             \
   fflush(log_fp);


#define VHOST_MSG(...)                         \
   if (verbosity=="high" || verbosity=="full") \
   {                                           \
   printf ("\n(vmsg) " __VA_ARGS__);           \
   fprintf (log_fp, "\n(vmsg) " __VA_ARGS__);  \
   fflush(stdout);                             \
   fflush(log_fp);                             \
   }


#define DBUG_MSG(...)                          \
   if (verbosity=="full")                      \
   {                                           \
   printf ("\n(dbug) " __VA_ARGS__);           \
   fprintf (log_fp, "\n(dbug) " __VA_ARGS__);  \
   fflush(stdout);                             \
   fflush(log_fp);                             \
   }


#define IXIA_MSGNOF(...)                       \
   printf (__VA_ARGS__);                       \
   fprintf (log_fp, __VA_ARGS__);              \
   fflush(stdout);                             \
   fflush(log_fp);


//---------------------
// Vhost-User Specific
//---------------------
#define VHOST_USER_PROTOCOL_F_MQ          0
#define VHOST_USER_PROTOCOL_F_LOG_SHMFD   1
#define VHOST_USER_PROTOCOL_F_RARP        2
#define VHOST_MEMORY_MAX_NREGIONS         8
#define VHOST_USER_VERSION_MASK           0x3
#define VHOST_USER_REPLY_MASK             (0x1 << 2)
#define VHOST_USER_VRING_IDX_MASK         0xff
#define VHOST_USER_VRING_NOFD_MASK        (0x1<<8)
#define VHOST_USER_HDR_SIZE               offsetof(vu_msg, payload.u64)
#define VHOST_VRING_F_LOG                 0
#define VHOST_USER_VERSION                0x1
#define VHOST_F_LOG_ALL                   26
#define VHOST_USER_F_PROTOCOL_FEATURES    30
#define VHOST_LOG_PAGE                    4096


//-----------------
// VirtIO Specific
//-----------------
#define VIRTIO_NET_F_MRG_RXBUF      15
#define VIRTIO_NET_F_GUEST_ANNOUNCE 21
#define VIRTIO_F_VERSION_1          32
#define MAX_NR_VIRTQUEUE            8


//----------------
// Vring Specific
//----------------
#define VRING_AVAIL_F_NO_INTERRUPT 1
#define VRING_DESC_F_WRITE         2
#define VRING_DESC_F_NEXT          1


//---------------
// Mmap Specific
//---------------
#define PROT_READ   0x1
#define PROT_WRITE  0x2
#define PROT_EXEC   0x4
#define PROT_NONE   0x0
#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02


//------------------------
// Ixia Metadata Specific
//------------------------
#define PALLADIUM_LATENCY       0x2ull
#define PALLADIUM_IFG           0x1ull
#define PALLADIUM_ET            0x4ull
#define PALLADIUM_PORT_DISABLED 0x8ull

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


//-----------------------
// Vhost-User Request Id
//-----------------------
enum vu_request 
   {
   VHOST_USER_NONE                  =  0,
   VHOST_USER_GET_FEATURES          =  1,
   VHOST_USER_SET_FEATURES          =  2,
   VHOST_USER_SET_OWNER             =  3,
   VHOST_USER_RESET_OWNER           =  4,
   VHOST_USER_SET_MEM_TABLE         =  5,
   VHOST_USER_SET_LOG_BASE          =  6,
   VHOST_USER_SET_LOG_FD            =  7,
   VHOST_USER_SET_VRING_NUM         =  8,
   VHOST_USER_SET_VRING_ADDR        =  9,
   VHOST_USER_SET_VRING_BASE        = 10,
   VHOST_USER_GET_VRING_BASE        = 11,
   VHOST_USER_SET_VRING_KICK        = 12,
   VHOST_USER_SET_VRING_CALL        = 13,
   VHOST_USER_SET_VRING_ERR         = 14,
   VHOST_USER_GET_PROTOCOL_FEATURES = 15,
   VHOST_USER_SET_PROTOCOL_FEATURES = 16,
   VHOST_USER_GET_QUEUE_NUM         = 17,
   VHOST_USER_SET_VRING_ENABLE      = 18,
   VHOST_USER_SEND_RARP             = 19,
   VHOST_USER_MAX
   };


enum adaptor_block
{
   ixia_block      = 0,
   avip_data_block = 1,
   avip_ctrl_block = 2
};


//-------------------------------
// Vhost-User Request Id Strings
//-------------------------------
static const char *vu_request_str[] = 
   {
   "vhost_user_none"                  ,
   "vhost_user_get_features"          ,
   "vhost_user_set_features"          ,
   "vhost_user_set_owner"             ,
   "vhost_user_reset_owner"           ,
   "vhost_user_set_mem_table"         ,
   "vhost_user_set_log_base"          ,
   "vhost_user_set_log_fd"            ,
   "vhost_user_set_vring_num"         ,
   "vhost_user_set_vring_addr"        ,
   "vhost_user_set_vring_base"        ,
   "vhost_user_get_vring_base"        ,
   "vhost_user_set_vring_kick"        ,
   "vhost_user_set_vring_call"        ,
   "vhost_user_set_vring_err"         ,
   "vhost_user_get_protocol_features" ,
   "vhost_user_set_protocol_features" ,
   "vhost_user_get_queue_num"         ,
   "vhost_user_set_vring_enable"      ,
   "vhost_user_send_rarp"             ,
   "vhost_user_max"                   ,
   };


//--------------------------------
// Ixia Cadence specific Metadata
//--------------------------------
struct palladium_metadata 
   {
   uint64_t metadata_type;
   union 
      {
      uint64_t ifg;
      uint64_t emulator_time;
      };
   uint64_t latency;
   char reserved[21];
   }
   __attribute__((__packed__));


//-----------------------------------
// IxVerify per frame control header
//-----------------------------------
struct ixverify_metadata_hdr 
   {
   uint16_t mrg_num_buffers;
   uint8_t  generic_flags;
   palladium_metadata pd;
   }
   __attribute__((__packed__));


//----------------- 
// Vring Structures   
//----------------- 
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


struct vh_vring_state 
   {
   unsigned int index;
   unsigned int num;
   };


struct vh_vring_addr 
   {
   unsigned int index;
   unsigned int flags;
   uint64_t     desc_user_addr;
   uint64_t     used_user_addr;
   uint64_t     avail_user_addr;
   };


//-----------------------
// Vhost-User structures
//-----------------------
struct vu_mem_region 
   {
   uint64_t guest_phys_addr;
   uint64_t memory_size;
   uint64_t userspace_addr;
   uint64_t mmap_offset;
   };


struct vu_mem 
   {
   uint32_t nregions;
   uint32_t padding;
   vu_mem_region regions[VHOST_MEMORY_MAX_NREGIONS];
   };


struct vu_log 
   {
   uint64_t mmap_size;
   uint64_t mmap_offset;
   };


struct vu_msg 
   {
   vu_request request;
   uint32_t   flags;
   uint32_t   size;
   union 
      {
      uint64_t              u64;
      struct vh_vring_state state;
      struct vh_vring_addr  addr;
      vu_mem                memory;
      vu_log                log;
      } 
      payload;
   int fds[VHOST_MEMORY_MAX_NREGIONS];
   }
   __attribute((packed));


//-----------------------------------------
// Top level virtque and device structures
//-----------------------------------------
struct vh_device_region 
   {
   uint64_t gpa;
   uint64_t size;
   uint64_t qva;
   uint64_t mmap_offset;
   uint64_t mmap_addr;
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


struct mrgbuf_db 
   {
   vector<uint64_t> addr;
   vector<uint32_t> chunk_len;
   vector<uint32_t> write_len;
   vector<uint16_t> index;
   }; 


struct port_connection_info
{
   int ixia_socket_server_fd; // updated by ixia device
   int ixia_socket_client_fd; // per port request from vlm
   int port_num;              // port number
   int flag;                  // used or not
   avip_driver *driver;       // updated by avip driver
   int avip_socket_client_fd; // per port request from avip
};


// 0:non-blocking mode, 1:blocking mode
int fd_set_blocking(int fd, int blocking) 
{
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1)
       return 0;
   if (blocking)
       flags &= ~O_NONBLOCK;
   else
       flags |= O_NONBLOCK;
   return fcntl(fd, F_SETFL, flags) != -1;
}


void display_usage_and_exit()
{
   stringstream os;
   os.str("");
   os <<
   "Ixia Avip Adaptor (v-" << version << ")  \n\n" 
   "Usage: \n"
   "  ixia_cdn_adaptor [-options=<value>] \n"
   "\n"
   "Options: \n"
   "  -ixia_vlm_path=<filename>   Path of the domain socket that will be used by adaptor to connect to Ixia Load module. \n"
   "                              This path should be passed on to the chardev when invoking Ixia load module\n"
   "  -avip_data_path=<filename>  Path of the domain socket that will be used by AVIP for data transfers. To be set only when mode=local \n"
   "                              This path should be passed on to the AVIP Proxy\n"
   "  -avip_ctrl_path=<filename>  Path of the domain socket that will be used by AVIP for ctrl messages. To be set only when mode=local \n"
   "                              This path should be passed on to the AVIP Proxy\n"
   "  -log=<filename>             Optional. To specify a custom log filename. Default is ./ixia_cdn_adaptor.log \n"
   "  -verbosity=<low/high/full>  Optional. Default verbosity is low, high enables vhost messages, full enables vhost and debug messages \n"
   "  -mode=<local/remote>        local=Single Workstation; remote=multiple workstation. Default mode is local \n"
   "                              remote mode is not supported in this release\n"
   "  -port=<#>                   Specified when mode=remote. This is the TCP socket port number that will be used on the Ixia Server \n"
   "  -help                       Display this help \n"
   ;
   printf ("%s", os.str().c_str());
   exit (EXIT_FAILURE);
}


class avip_driver 
{
   public://driver fn
      int process_vhost_message (int fd);
      avip_driver (string mode);
      void start_tx_thread();
      void start_rx_thread();
      void stop_all_threads();
      void tx_thread();
      void rx_thread();
      int port_num;
      bool is_connected;
      unsigned int tx_frame_cnt;
      unsigned int rx_frame_cnt;
      ixia_device *adaptor;
      int sock;
      void set_poll_interval(unsigned int interval);
   private://driver vars
      bool stop_tx;
      bool stop_rx;
      thread tx;
      thread rx;
      unsigned char *txbuf;
      unsigned char *rxbuf;
      int metadata_len;
      uint64_t emu_time;
      uint64_t latency;
   private://driver fn
      void send_to_ixia (uint8_t *buf, uint32_t len, uint64_t ts, uint64_t latency);
      void send_to_avip (uint8_t *buf, uint32_t len);
      void process_frames_from_ixia();
      void process_frames_from_avip();
      int extract_one_frame_sock();
      int extract_one_frame_vq();
      void reset();
   private://vhost fn
      uint64_t gpa_to_va(uint64_t guest_addr, int mode);
      uint64_t qva_to_va(uint64_t qemu_addr);
      void kick_ixia(vh_virtqueue *vq);
      void send_reply_to_ixia(int fd);
   private://vhost vars
      vh_device_region regions[VHOST_MEMORY_MAX_NREGIONS];
      ixia_device *device;
      uint64_t features;
      uint32_t nregions;
      vh_virtqueue vq[2];
      mrgbuf_db mdb;
      vu_msg vmsg;
      int hdrlen;
      string mode;
      int ready;
      int d_index_running;
      unsigned int poll_interval;
};


class ixia_device
{
   public: //fn
      ixia_device (
            string ixia_vlm_path, 
            string avip_data_path, 
            string avip_ctrl_path, 
            string mode, 
            unsigned int poll_interval
            );
      // creating socket server
      bool init_ixia_server ();
      bool init_avip_data_server ();
      bool init_avip_ctrl_server ();
      // polling sockets
      void start_ixia_server();
      void start_avip_data_server();
      void start_avip_ctrl_server();
      void join_threads();
   private://fn
      int add_fd (adaptor_block block, int fd, int type=0); // 0=client 1=server
      int add_port (adaptor_block block, int fd);
      void reset ();
      void clean_ixia();
      void clean_avip();
      // while loop
      void ixia_server();
      void avip_data_server();
      void avip_ctrl_server();
      // next event
      int process_ixia_events(uint32_t timeout);
      int process_avip_data_event(uint32_t timeout);
      int process_avip_ctrl_event(uint32_t timeout);
      int get_the_free_index(); 
   private://vars
      // fdset
      fd_set ixia_fdset;
      fd_set avip_data_fdset;
      fd_set avip_ctrl_fdset;
      int ixia_max_fd;
      int avip_data_max_fd;
      int avip_ctrl_max_fd;
      // server fd
      int ixia_server_fd;
      int avip_data_server_fd;
      int avip_ctrl_server_fd;
      int avip_ctrl_client_fd;
      // sockets address
      struct sockaddr_un ixia_server_addr;
      struct sockaddr_un avip_data_server_addr;
      struct sockaddr_un avip_ctrl_server_addr;
      // socket paths
      string ixia_vlm_path;
      string avip_data_path;
      string avip_ctrl_path;
      // local or remote
      string mode;
      // threads
      thread ixia_vhost_server_thread;
      thread avip_data_server_thread;
      thread avip_ctrl_server_thread;
      // to store connection info for each port
      map <int, port_connection_info*> port;
      int port_num;
      unsigned int poll_interval;
};


void avip_driver::set_poll_interval(unsigned int interval)
{
   poll_interval = interval;
}

void avip_driver::start_tx_thread() 
{
   stop_tx = false;
   tx = thread (&avip_driver::tx_thread, this);
   char thread_name[80];
   sprintf(thread_name, "TXthread_%d", port_num);
   pthread_setname_np(tx.native_handle(), thread_name);
   tx.detach();
}


void avip_driver::tx_thread() 
{
   AVIP_MSG("Port:%d Tx Thread Started ", port_num);
   while (!stop_tx) 
   {
      process_frames_from_ixia();
      usleep(poll_interval);
   }
   AVIP_MSG("Port:%d Tx Thread Stopped ", port_num);
};


void avip_driver::process_frames_from_ixia()
{
   struct vring_avail *avail = vq[1].avail;
   struct vring_used *used =   vq[1].used;

   if (vq[1].last_avail_idx != atomic_fetch_add (&avail->idx,0))
   {
      extract_one_frame_vq();
      vq[1].last_avail_idx++;
      vq[1].last_used_idx++;
   }
   atomic_xchg (&used->idx, vq[1].last_used_idx);
}


int avip_driver::extract_one_frame_vq()
{
   struct vring_desc *desc   = vq[1].desc;
   struct vring_avail *avail = vq[1].avail;
   struct vring_used *used   = vq[1].used;
   
   unsigned int size = vq[1].size;
   uint16_t a_index = vq[1].last_avail_idx % size;
   uint16_t u_index = vq[1].last_used_idx % size;
   uint16_t d_index = avail->ring[a_index];
   
   uint32_t i, len = 0;
   size_t buf_size = MAX_FRAME_SIZE;

   i = d_index;
   do {
      void *chunk_start = (void *)(uintptr_t)gpa_to_va(desc[i].addr, 1);
      uint32_t chunk_len = desc[i].len;

      assert(!(desc[i].flags & VRING_DESC_F_WRITE));
   
      if (len + chunk_len < buf_size) {
         memcpy(txbuf+len, chunk_start, chunk_len);
      }
      else {
         AVIP_MSG ("Error: too long packet. Dropping...");
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

   send_to_avip(txbuf, len);
   return 0;
}


void avip_driver::send_to_avip (uint8_t *buf, uint32_t len)
{
   ixverify_metadata_hdr meta = {0};
   memcpy (&meta, buf, metadata_len);
   uint32_t ipg = 0;
   uint32_t final_len = 0;
   
   if (!(meta.generic_flags))
   {
      tx_frame_cnt++;
      ipg = meta.pd.ifg;

      // overwite len and ipg on the last 8 bytes of metadata
      final_len = len-metadata_len;
      memcpy ((buf+40), (char*)&final_len, FRAME_HDR_LEN_SIZE);
      memcpy ((buf+40)+FRAME_HDR_LEN_SIZE, (char*)&ipg, FRAME_HDR_IPG_SIZE);
      
      AVIP_MSG ("PORT:%d Sending a frame from ixia to avip -> Len=%d (%d, ipg=%d)", port_num, final_len, tx_frame_cnt, ipg);
      send (sock, buf+(metadata_len-8), final_len+8, 0);
   }
}


void avip_driver::start_rx_thread() 
{
   stop_rx = false;
   rx = thread (&avip_driver::rx_thread, this);
   char thread_name[80];
   sprintf(thread_name, "RXthread_%d", port_num);
   pthread_setname_np(rx.native_handle(), thread_name);
   rx.detach();
}

// void avip_driver::rx_thread()
// {
//    AVIP_MSG("Port:%d Rx Thread Started ", port_num);
// 
//    fd_set readset;
//    fd_set tmp_set;
//    FD_ZERO (&readset);
//    FD_SET (sock, &readset);
//    int fdmax = sock;
//    unsigned int timeout = 200000;
//    char buf[256];
// 
//    while (!stop_rx)
//    {
//       // AVIP_MSG("checking sock");
//       struct timeval tv;
//       tv.tv_sec = timeout / 1000000;
//       tv.tv_usec = timeout % 1000000;
//       FD_ZERO (&tmp_set);
//       tmp_set = readset;
//       if (select (fdmax+1, &tmp_set, NULL, NULL, &tv) == -1) {
//          perror ("Select Error");
//          exit(1);
//       }
//       int fd;
//       for (fd=0; fd<=fdmax; fd++) {
//          if (fd == sock) {
//             process_frames_from_avip();
//          }
//       }
//    }
//    AVIP_MSG("Port:%d Rx Thread Stopped ", port_num);
// };


void avip_driver::rx_thread() 
{
   AVIP_MSG("Port:%d Rx Thread Started ", port_num);
   while (!stop_rx) 
   {
      process_frames_from_avip();
      // usleep(poll_interval);
      usleep(1000);
   }
   AVIP_MSG("Port:%d Rx Thread Stopped ", port_num);
};


void avip_driver::process_frames_from_avip()
{
   int flen=MAX_FRAME_SIZE;
   int num_bytes = recv (sock, rxbuf, MAX_FRAME_SIZE, 0);
   if (num_bytes>0)
   {
      memcpy (&flen,     rxbuf,    4);
      memcpy (&emu_time, rxbuf+4,  8);
      memcpy (&latency,  rxbuf+12, 8);
      rx_frame_cnt++;
      AVIP_MSG ("PORT:%d Sending a frame from avip to ixia <- Len=%d (%d, ts=%lu, lat=%lu) ", port_num, flen, rx_frame_cnt, emu_time, latency);
      send_to_ixia ((uint8_t*) rxbuf+20, flen, emu_time, latency);
   }
}


void avip_driver::send_to_ixia (uint8_t *buf, uint32_t len, uint64_t ts, uint64_t latency)
{
   vh_virtqueue *rx_vq = &vq[0];

   struct vring_desc  *desc  = rx_vq->desc;
   struct vring_avail *avail = rx_vq->avail;
   struct vring_used  *used  = rx_vq->used;

   int32_t  remaining_len = len;
   uint32_t size          = rx_vq->size;
   uint16_t avail_index   = atomic_fetch_add(&avail->idx,0);

    if (rx_vq->last_avail_idx == avail_index) {
        AVIP_MSG("No available descriptors on RX virtq.\n");
        return;
    }

   uint16_t a_index = rx_vq->last_avail_idx % size;
   uint16_t u_index = rx_vq->last_used_idx % size;
   uint16_t d_index = atomic_fetch_add(&avail->ring[a_index],0);

   int nbufs = (ceil) ((float) (len+48)/4096);

   struct ixverify_metadata_hdr meta = {0};
   meta.mrg_num_buffers = nbufs;
   meta.pd.emulator_time = ts;
   meta.pd.latency = latency;
   meta.pd.metadata_type = PALLADIUM_ET | PALLADIUM_LATENCY;
   meta.generic_flags = 0;

   mdb.addr.clear();
   mdb.chunk_len.clear();
   mdb.write_len.clear();
   mdb.index.clear();

   int i = d_index;
   int nbufs_cnt=0;
   do {
      //-------------------------
      // Collect free descriptor
      //-------------------------
      if (!(desc[i].flags & VRING_DESC_F_WRITE)) 
         {
         AVIP_MSG ("Error: descriptor is not writable. Exiting");
         AVIP_MSG ("Dropping Index=%d",i);
         break;
         }
      else 
         {
         mdb.addr.push_back (desc[i].addr);
         mdb.chunk_len.push_back (desc[i].len);
         mdb.index.push_back (i);
         nbufs_cnt++;
         AVIP_MSG ("desc i=%d addr=%x len=%d flag=%d",i, desc[i].addr, desc[i].len, desc[i].flags);
         }

      //----------------------------
      // next available descriptors
      //----------------------------
      if ((desc[i].flags & VRING_DESC_F_NEXT))
         {
         i = atomic_fetch_add (&desc[i].next,0);
         AVIP_MSG ("Desc chain Index=%d",i);
         }
      else
         {
         a_index = a_index+1;
         i = atomic_fetch_add(&avail->ring[(a_index%size)],0);
         AVIP_MSG ("Avail Index=%d",i);
         }
      } 
   while (nbufs != nbufs_cnt);

   assert (nbufs_cnt==nbufs);

   uint32_t written_len = 0;
   unsigned int chunk_len;
   unsigned int chunk_to_write;

   for (int cnt=0; cnt<nbufs; cnt++)
   {
      uint64_t desc_addr = gpa_to_va (mdb.addr[cnt], 0);
      if (cnt==0)
         {
         chunk_to_write = metadata_len;
         // write to desc
         memcpy ((void *)((uintptr_t)(desc_addr)), &meta, metadata_len);
         chunk_to_write = MIN(remaining_len, mdb.chunk_len[cnt]-metadata_len);
         remaining_len = remaining_len - chunk_to_write;
         // write to desc
         memcpy ((void *)((uintptr_t)(desc_addr+metadata_len)), buf, chunk_to_write);
         written_len = written_len + chunk_to_write;
         atomic_xchg (&used->ring[u_index].id , mdb.index[cnt]);
         atomic_xchg (&used->ring[u_index].len , chunk_to_write+metadata_len);
         }
      else
         {
         chunk_to_write = MIN(remaining_len, mdb.chunk_len[cnt]);
         remaining_len = remaining_len - chunk_to_write;
         // write to desc
         memcpy ((void *)((uintptr_t)(desc_addr)), buf+written_len, chunk_to_write);
         written_len = written_len + chunk_to_write;
         atomic_xchg (&used->ring[u_index].id , mdb.index[cnt]);
         atomic_xchg (&used->ring[u_index].len , chunk_to_write);
         }

      rx_vq->last_avail_idx++;
      rx_vq->last_used_idx++;
      atomic_xchg (&used->idx, rx_vq->last_used_idx);
      u_index = rx_vq->last_used_idx % size;
   }
   kick_ixia(rx_vq);
}


void avip_driver::stop_all_threads()
{
   stop_tx=true;
   stop_rx=true;
}


avip_driver::avip_driver (string m)
{
   reset();
   this->mode = mode;
}


void avip_driver::reset()
{
   txbuf = new unsigned char [MAX_FRAME_SIZE];
   rxbuf = new unsigned char [MAX_FRAME_SIZE];
   metadata_len = METADATA_LENGHT;
   is_connected = false;
   d_index_running = 0;
   poll_interval = 1;
   tx_frame_cnt = 0;
   rx_frame_cnt = 0;
   stop_tx  = true;
   stop_rx  = true;
   device   = NULL;
   emu_time = 0;
   features = 0;
   nregions = 0;
   port_num = 0;
   latency  = 0;
   hdrlen   = 0;
   ready    = 0;
   sock     = -1;
   mode.clear();
}


void avip_driver::kick_ixia (vh_virtqueue *vq)
{
   if (!(vq->avail->flags & VRING_AVAIL_F_NO_INTERRUPT)) 
      eventfd_write(vq->call_fd, 1);
}


uint64_t avip_driver::qva_to_va (uint64_t qemu_addr)
{
   uint32_t i;
   for (i = 0; i < nregions; i++) 
   {
      vh_device_region *r = &regions[i];
      if ((qemu_addr >= r->qva) && (qemu_addr < (r->qva + r->size))) 
         return qemu_addr - r->qva + r->mmap_addr + r->mmap_offset;
   }
   assert(!"address not found in regions\n");
   return 0;
}


uint64_t avip_driver::gpa_to_va (uint64_t guest_addr, int mode)
{
   uint32_t i;
   for (i = 0; i < nregions; i++) 
   {
      vh_device_region *r = &regions[i];
      if ((guest_addr >= r->gpa) && (guest_addr < (r->gpa + r->size))) 
         return guest_addr - r->gpa + r->mmap_addr + r->mmap_offset;
   }
   if (mode==1) assert(!"Transmit side address not found in regions\n");
   if (mode==0) assert(!"Receive side address not found in regions\n");
   return 0;
}


void avip_driver::send_reply_to_ixia (int fd)
{
   int rc;
   do {
      rc = write (fd, &vmsg, VHOST_USER_HDR_SIZE + vmsg.size);
   } 
   while (rc < 0 && errno == EINTR);

   if (rc < 0) 
   {
      AVIP_MSG ("Write Error in send_reply_to_ixia");
      exit(1);
   }
}


int avip_driver::process_vhost_message (int fd)
{
   char control[CMSG_SPACE(VHOST_MEMORY_MAX_NREGIONS * sizeof(int))] = { };

   struct iovec iov;
   iov.iov_base = (char *)&vmsg;
   iov.iov_len = offsetof(vu_msg, payload.u64);

   struct msghdr msg;
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1;
   msg.msg_control = control;
   msg.msg_controllen = sizeof(control);

   size_t fd_size;
   struct cmsghdr *cmsg;
   uint32_t rc;

   rc = recvmsg (fd, &msg, 0);

   if (rc == 0)
   {
      IXIA_MSG("Load Module disconnected");
      // return 100;
      exit(1);
   }

   if (rc < 0) 
   {
      VHOST_MSG("Execute Request : rc<0");
      return 0;
      exit(1);
   }

   for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
   {
      if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
      {
         fd_size = cmsg->cmsg_len - CMSG_LEN(0);
         memcpy(vmsg.fds, CMSG_DATA(cmsg), fd_size);
         break;
      }
   }

   if (vmsg.size > sizeof(vmsg.payload))
   {
      VHOST_MSG ("Error: too big message request: %d, size: vmsg.size: %u, "
      "while sizeof(vmsg.payload) = %zu\n",
      vmsg.request, vmsg.size, sizeof(vmsg.payload));
      exit(1);
   }

   if (vmsg.size)
   {
      rc = read(fd, &vmsg.payload, vmsg.size);
      if (rc == 0)
      {
         IXIA_MSG("Load Module disconnected");
         // return 100;
         exit(1);
      }
      if (rc < 0) 
      {
         VHOST_MSG("Execute Request : rc<0");
         return 0;
         exit(1);
      }
      assert(rc == vmsg.size);
   }

   if (vmsg.request != 0)
   {
   VHOST_MSG("Vhost msg rcvd (%s)",  vu_request_str[vmsg.request] );
   }

   switch (vmsg.request)
   {
      case VHOST_USER_GET_FEATURES:{
         vmsg.payload.u64 =
            ((1ULL << VIRTIO_NET_F_MRG_RXBUF) |
            (1ULL << VHOST_F_LOG_ALL) |
            (1ULL << VIRTIO_NET_F_GUEST_ANNOUNCE) |
            (1ULL << VHOST_USER_F_PROTOCOL_FEATURES));
         vmsg.size = sizeof(vmsg.payload.u64);
         vmsg.flags &= ~VHOST_USER_VERSION_MASK;
         vmsg.flags |= VHOST_USER_VERSION;
         vmsg.flags |= VHOST_USER_REPLY_MASK;
         send_reply_to_ixia (fd);
         return 1;
         break;
         }

      case VHOST_USER_SET_VRING_CALL:{
         uint64_t u64_arg = vmsg.payload.u64;
         VHOST_MSG("-> u64: 0x%llx", vmsg.payload.u64);
         int index = u64_arg & VHOST_USER_VRING_IDX_MASK;
         assert((u64_arg & VHOST_USER_VRING_NOFD_MASK) == 0);
         vq[index].call_fd = vmsg.fds[0];
         VHOST_MSG("-> Got call_fd: %d for vq: %d", vmsg.fds[0], index);
         return 0;
         break;
         }

      case VHOST_USER_SET_FEATURES:{
         VHOST_MSG("-> u64: 0x%llx", vmsg.payload.u64);
         features = vmsg.payload.u64;
         if ((features & (1ULL << VIRTIO_F_VERSION_1)) ||
             (features & (1ULL << VIRTIO_NET_F_MRG_RXBUF))) {
             hdrlen = 12;
         } else {
             hdrlen = 10;
         }
         return 0;
         break;
         }

      case VHOST_USER_SET_MEM_TABLE:{
         vu_mem *memory = &vmsg.payload.memory;
         nregions = memory->nregions;
         VHOST_MSG("-> Nregions: %d", memory->nregions);
         uint32_t i;

         for (i = 0; i < nregions; i++)
         {
            void *mmap_addr;
            vu_mem_region *msg_region = &memory->regions[i];
            vh_device_region *dev_region = &regions[i];
            VHOST_MSG("-> Region %d", i);
            VHOST_MSG("--> guest_phys_addr: 0x%llx", msg_region->guest_phys_addr);
            VHOST_MSG("--> memory_size: 0x%llx", msg_region->memory_size);
            VHOST_MSG("--> userspace_addr: 0x%llx", msg_region->userspace_addr);
            VHOST_MSG("--> mmap_offset: 0x%llx", msg_region->mmap_offset);
            dev_region->gpa = msg_region->guest_phys_addr;
            dev_region->size = msg_region->memory_size;
            dev_region->qva = msg_region->userspace_addr;
            dev_region->mmap_offset = msg_region->mmap_offset;
            mmap_addr = mmap (
               0, dev_region->size + dev_region->mmap_offset,
               PROT_READ | PROT_WRITE, MAP_SHARED, vmsg.fds[i],
               0
               );
            if (mmap_addr == MAP_FAILED) {
               VHOST_MSG("-> MMap Failed\n");
               exit(1);
            }
            dev_region->mmap_addr = (uint64_t)(uintptr_t)mmap_addr;
            VHOST_MSG("--> mmap_addr: 0x%llx", dev_region->mmap_addr);
            close(vmsg.fds[i]);
         }
         return 0;
         break;
         }

      case VHOST_USER_SET_VRING_NUM:{
         unsigned int index = vmsg.payload.state.index;
         unsigned int num = vmsg.payload.state.num;
         VHOST_MSG("-> State.index: %d", index);
         VHOST_MSG("-> State.num: %d", num);
         vq[index].size = num;
         return 0;
         break;
         }

      case VHOST_USER_SET_VRING_BASE:{
         unsigned int index = vmsg.payload.state.index;
         unsigned int num = vmsg.payload.state.num;
         VHOST_MSG("-> State.index: %d", index);
         VHOST_MSG("-> State.num: %d", num);
         vq[index].last_avail_idx = num;
         return 0;
         break;
         }

      case VHOST_USER_SET_VRING_ADDR:{
         struct vh_vring_addr *vra = &vmsg.payload.addr;
         unsigned int index = vra->index;
         vh_virtqueue *vqi = &vq[index];
         VHOST_MSG("-> vh_vring_addr:");
         VHOST_MSG("--> index: %d", vra->index);
         VHOST_MSG("--> flags: %d", vra->flags);
         VHOST_MSG("--> desc_user_addr: 0x%016lx", vra->desc_user_addr);
         VHOST_MSG("--> used_user_addr: 0x%016lx", vra->used_user_addr);
         VHOST_MSG("--> avail_user_addr: 0x%016lx", vra->avail_user_addr);
         vqi->desc = (struct vring_desc *)(uintptr_t)qva_to_va  (vra->desc_user_addr);
         vqi->used = (struct vring_used *)(uintptr_t)qva_to_va  (vra->used_user_addr);
         vqi->avail = (struct vring_avail *)(uintptr_t)qva_to_va(vra->avail_user_addr);
         VHOST_MSG("-> Setting virtq addresses:");
         VHOST_MSG("--> vring_desc at %p", vqi->desc);
         VHOST_MSG("--> vring_used at %p", vqi->used);
         VHOST_MSG("--> vring_avail at %p", vqi->avail);
         vqi->last_used_idx = vqi->used->idx;
         if (vqi->last_avail_idx != vqi->used->idx) {
            VHOST_MSG("--> Last avail index != used index: %d != %d, resuming", vqi->last_avail_idx, vqi->used->idx);
            vqi->last_avail_idx = vqi->used->idx;
         } 
         return 0;
         break;
         }

      case VHOST_USER_SET_VRING_KICK:{
         uint64_t u64_arg = vmsg.payload.u64;
         int index = u64_arg & VHOST_USER_VRING_IDX_MASK;
         vq[index].kick_fd = vmsg.fds[0];
         VHOST_MSG("-> Got kick_fd: %d for vq: %d", vmsg.fds[0], index);
         if (index % 2 == 1) {
         }
         if (vq[0].kick_fd != -1 && vq[1].kick_fd != -1) {
            ready = 1;
            VHOST_MSG ("-> PORT:%d is ready", port_num);
         }
         return 0;
         break;
         }

      case VHOST_USER_SET_LOG_BASE:{
         int fd;
         void *rc;
         fd = vmsg.fds[0];
         vmsg.size = sizeof(vmsg.payload.u64);
         vmsg.flags &= ~VHOST_USER_VERSION_MASK;
         vmsg.flags |= VHOST_USER_VERSION;
         vmsg.flags |= VHOST_USER_REPLY_MASK;
         send_reply_to_ixia (fd);
         return 1;
         break;
         }

      case VHOST_USER_SET_LOG_FD:{
         return 0;
         break;
         }

      case VHOST_USER_GET_VRING_BASE:{
         unsigned int index = vmsg.payload.state.index;
         VHOST_MSG("-> State.index: %d", index);
         vmsg.payload.state.num = vq[index].last_avail_idx;
         vmsg.size = sizeof(vmsg.payload.state);
         ready = 0;
         if (vq[index].call_fd != -1) {
            close(vq[index].call_fd);
            vq[index].call_fd = -1;
         }
         if (vq[index].kick_fd != -1) {
            close(vq[index].kick_fd);
            vq[index].kick_fd = -1;
         }
         vmsg.flags &= ~VHOST_USER_VERSION_MASK;
         vmsg.flags |= VHOST_USER_VERSION;
         vmsg.flags |= VHOST_USER_REPLY_MASK;
         send_reply_to_ixia (fd);
         return 1;
         break;
         }

      case VHOST_USER_GET_PROTOCOL_FEATURES:{
         vmsg.payload.u64 = 1ULL << VHOST_USER_PROTOCOL_F_LOG_SHMFD;
         VHOST_MSG("-> u64: 0x%llx", vmsg.payload.u64);
         vmsg.size = sizeof(vmsg.payload.u64);
         vmsg.flags &= ~VHOST_USER_VERSION_MASK;
         vmsg.flags |= VHOST_USER_VERSION;
         vmsg.flags |= VHOST_USER_REPLY_MASK;
         send_reply_to_ixia (fd);
         return 1;
         break;
         }

      case VHOST_USER_SET_VRING_ENABLE:{
         unsigned int index = vmsg.payload.state.index;
         unsigned int enable = vmsg.payload.state.num;
         VHOST_MSG("-> State.index: %d", index);
         VHOST_MSG("-> State.enable: %d", enable);
         vq[index].enable = enable;
         return 0;
         break;
         }

      case VHOST_USER_NONE:{
         return 0;
         break;
         }

      case VHOST_USER_SET_OWNER:{
         return 0;
         break;
         }

      case VHOST_USER_RESET_OWNER:{
         ready = 0;
         features = 0;
         return 0;
         break;
         }

      case VHOST_USER_SET_VRING_ERR:{
         VHOST_MSG("-> u64: 0x%llx", vmsg.payload.u64);
         return 0;
         break;
         }

      case VHOST_USER_SET_PROTOCOL_FEATURES:{
         VHOST_MSG("-> Not Implemented u64: 0x%llx", vmsg.payload.u64);
         return 0;
         break;
         }

      case VHOST_USER_SEND_RARP:{
         return 0;
         break;
         }

      case VHOST_USER_GET_QUEUE_NUM:{
         return 0;
         break;
         }

      case VHOST_USER_MAX:{
         assert(vmsg.request != VHOST_USER_MAX);
         return 0;
         break;
         }

      default:{
         VHOST_MSG("-> Invalid Request");
         break;
         }
   }
}


bool ixia_device::init_ixia_server () 
{
   if ((ixia_server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
   {
      IXIA_MSG ("Vhost Socket creation failed. Aborting...");
      exit (0);
   }
   socklen_t len;
   len = sizeof(struct sockaddr_un);
   ixia_server_addr.sun_family = AF_UNIX;
   strncpy (ixia_server_addr.sun_path, ixia_vlm_path.c_str(), (len - sizeof(short)));
   unlink (ixia_server_addr.sun_path);

   if ((::bind(ixia_server_fd, (struct sockaddr*) &ixia_server_addr, len)) == -1) 
   {
      IXIA_MSG ("Vhost Socket Bind failed. Aborting...");
      exit (0);
   }
   listen (ixia_server_fd, 256);
   add_fd (ixia_block, ixia_server_fd, 1);
   IXIA_MSG ("Vhost Socket Connection established");
   IXIA_MSG ("Waiting for Connection requests from Vlm...");
}


void ixia_device::start_ixia_server() 
{
   ixia_vhost_server_thread = thread (&ixia_device::ixia_server, this);
   pthread_setname_np(ixia_vhost_server_thread.native_handle(), "IxiaServer");
}


void ixia_device::ixia_server()
{
   IXIA_MSG ("Vhost Server Started");
   uint32_t timeout = 200000;
   while (1) {
      process_ixia_events (timeout);
   }
}


int ixia_device::process_ixia_events (uint32_t timeout)
{
   struct timeval tv;
   tv.tv_sec = timeout / 1000000;
   tv.tv_usec = timeout % 1000000;
   fd_set fdset_rd = ixia_fdset;
   int rc = select(ixia_max_fd+1, &fdset_rd, 0, 0, &tv);
   if (rc == -1) perror ("select"); 
   if (rc == 0) return 0;
   int fd;
   for (fd=0; fd<ixia_max_fd+1; fd++) 
   {
      if (FD_ISSET(fd, &fdset_rd) && FD_ISSET(fd, &ixia_fdset)) 
      {
         if (fd == ixia_server_fd)
            add_port (ixia_block, fd);
         else
            {
            int result = port[fd]->driver->process_vhost_message(fd);
            // if (result==100)
               // {
               // clean_ixia();
               // }
            }
      }
   } 
   return 0;
}


bool ixia_device::init_avip_data_server ()
{
   if ((avip_data_server_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
      IXIA_MSG ("AVIP Data Socket creation failed. Aborting...");
      exit (0);
   }
   socklen_t len;
   len = sizeof(struct sockaddr_un);
   avip_data_server_addr.sun_family = AF_UNIX;
   strncpy (avip_data_server_addr.sun_path, avip_data_path.c_str(), (len - sizeof(short)));
   unlink (avip_data_server_addr.sun_path);

   if ((::bind(avip_data_server_fd, (struct sockaddr*) &avip_data_server_addr, len)) == -1) 
   {
      IXIA_MSG ("AVIP Data Socket Bind failed. Aborting...");
      exit (0);
   }
   listen (avip_data_server_fd, 256);
   add_fd (avip_data_block, avip_data_server_fd, 1);
   AVIP_MSG("AVIP Data Socket Connection established");
   AVIP_MSG("Waiting for AVIP Ports to connect...");
}


void ixia_device::start_avip_data_server() 
{
   avip_data_server_thread = thread (&ixia_device::avip_data_server, this);
   pthread_setname_np(avip_data_server_thread.native_handle(), "AvipServer");
}


void ixia_device::avip_data_server()
{
   AVIP_MSG ("AVIP Data Server Started");
   uint32_t timeout = 200000;
   while (1) {
      process_avip_data_event (timeout);
   }
}


int ixia_device::get_the_free_index()
{
   int free_idx = 0;
   map <int, port_connection_info*>::iterator i;
   for (i=port.begin(); i!=port.end(); i++)
   {
      if (i->second->flag == 0)
      {
      free_idx = i->first;
      break;
      }
   }
   return free_idx;
}


int ixia_device::process_avip_data_event (uint32_t timeout)
{
   struct timeval tv;
   tv.tv_sec = timeout / 1000000;
   tv.tv_usec = timeout % 1000000;
   fd_set fdset_tmp = avip_data_fdset;
   int rc = select(avip_data_max_fd+1, &fdset_tmp, 0, 0, &tv);
   if (rc == -1) perror ("select"); 
   if (rc == 0) return 0;
   int fd;
   for (fd=0; fd<avip_data_max_fd+1; fd++) 
   {
      if (FD_ISSET(fd, &fdset_tmp) && FD_ISSET(fd, &avip_data_fdset)) 
      {
         if (fd == avip_data_server_fd)
         {
            add_port (avip_data_block, fd);
         }
      }
   }
   return 0;
}


bool ixia_device::init_avip_ctrl_server ()
{
   if ((avip_ctrl_server_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
      IXIA_MSG ("AVIP Control Socket creation failed. Aborting...");
      exit (0);
   }
   socklen_t len;
   len = sizeof(struct sockaddr_un);
   avip_ctrl_server_addr.sun_family = AF_UNIX;
   strncpy (avip_ctrl_server_addr.sun_path, avip_ctrl_path.c_str(), (len - sizeof(short)));
   unlink (avip_ctrl_server_addr.sun_path);

   if ((::bind(avip_ctrl_server_fd, (struct sockaddr*) &avip_ctrl_server_addr, len)) == -1) 
   {
      IXIA_MSG ("AVIP Control Socket Bind failed. Aborting...");
      exit (0);
   }
   listen (avip_ctrl_server_fd ,1);
   add_fd (avip_ctrl_block, avip_ctrl_server_fd, 1);
   AVIP_MSG("AVIP Control Socket Connection established");
   AVIP_MSG("Waiting for AVIP Test to connect...");
}


void ixia_device::start_avip_ctrl_server() 
{
   avip_ctrl_server_thread = thread (&ixia_device::avip_ctrl_server, this);
   pthread_setname_np(avip_ctrl_server_thread.native_handle(), "ControlServer");
}


void ixia_device::avip_ctrl_server()
{
   IXIA_MSG ("AVIP Control Server Started");
   uint32_t timeout = 200000;
   while (1) {
      process_avip_ctrl_event (timeout);
   }
}


int ixia_device::process_avip_ctrl_event (uint32_t timeout)
{
   struct timeval tv;
   tv.tv_sec = timeout / 1000000;
   tv.tv_usec = timeout % 1000000;
   fd_set fdset_rd = avip_ctrl_fdset;
   int rc = select(avip_ctrl_max_fd+1, &fdset_rd, 0, 0, &tv);
   if (rc == -1) perror ("select"); 
   if (rc == 0) return 0;
   int fd;
   for (fd=0; fd<avip_ctrl_max_fd+1; fd++) 
   {
      if (FD_ISSET(fd, &fdset_rd) && FD_ISSET(fd, &avip_ctrl_fdset)) 
      {
         if (fd == avip_ctrl_server_fd)
            add_port (avip_ctrl_block, fd);
         else if (fd==avip_ctrl_client_fd) // end of test
            clean_avip();
      }
   }
   return 0;
}


void ixia_device::clean_ixia()
{
   map <int, port_connection_info*>::iterator i;
   for (i=port.begin(); i!=port.end(); i++)
   {
      delete i->second->driver;
      close (i->second->ixia_socket_client_fd);
      delete i->second;
   }
   port.clear();
   // clear ixia fdset
   FD_ZERO (&ixia_fdset);
   FD_SET (ixia_server_fd, &ixia_fdset);
   // clear ixia fdmax
   ixia_max_fd = ixia_server_fd;
   // clear port number
   port_num = 0;
}


int ixia_device::add_port (adaptor_block block, int fd)
{
   switch (block)
   {
      case ixia_block: {
         struct sockaddr_un un;
         socklen_t len = sizeof(un);
         int ixia_client_fd = accept(ixia_server_fd, (struct sockaddr *) &un, &len);
         avip_driver *driver = new avip_driver(mode);
            driver->adaptor = this;
            driver->port_num = port_num;
         port_connection_info *p = new port_connection_info;
            p->flag = 0;
            p->ixia_socket_server_fd = ixia_server_fd;
            p->ixia_socket_client_fd = ixia_client_fd;
            p->port_num = port_num;
            p->avip_socket_client_fd = 0;
            p->driver = driver;
         port[ixia_client_fd] = p;
         add_fd (ixia_block, ixia_client_fd); 
         port_num++;
         break;
         }

      case avip_data_block:{
         struct sockaddr_un un;
         socklen_t len = sizeof(un);
         int avip_client_fd = accept(avip_data_server_fd, (struct sockaddr *) &un, &len);
         int free_idx = get_the_free_index();
         port[free_idx]->avip_socket_client_fd = avip_client_fd;
         port[free_idx]->driver->set_poll_interval(poll_interval);
         port[free_idx]->flag = 1;
         port[free_idx]->driver->sock = avip_client_fd;
         port[free_idx]->driver->tx_frame_cnt = 0;
         port[free_idx]->driver->rx_frame_cnt = 0;
         port[free_idx]->driver->is_connected = true;
         add_fd (avip_data_block, avip_client_fd); 
         port[free_idx]->driver->start_tx_thread();
         port[free_idx]->driver->start_rx_thread();
         break;
         }

      case avip_ctrl_block:{
         struct sockaddr_un un;
         socklen_t len = sizeof(un);
         avip_ctrl_client_fd = accept(avip_ctrl_server_fd, (struct sockaddr *) &un, &len);
         add_fd (avip_ctrl_block, avip_ctrl_client_fd);
         // TODO check if this recv is necessary
         char buf[256];
         int nbytes = recv (avip_ctrl_client_fd, buf, sizeof buf, 0);
         }
   } //switch
}


void ixia_device::clean_avip()
{

   // TODO 
   // before stopping all the threads flush all the unprocess frames
   // from the TX side of the virtqueue till the stop flag in metadata

   // clean avip data block
   map <int, port_connection_info*>::iterator i;
   for (i=port.begin(); i!=port.end(); i++)
   {
      i->second->flag = 0;
      int fd = i->second->avip_socket_client_fd;
      i->second->driver->stop_all_threads();
      i->second->avip_socket_client_fd = -1;
      i->second->driver->sock = -1;
      FD_CLR (fd, &avip_data_fdset);
      close (fd);
   }
   avip_data_max_fd = avip_data_server_fd;

   // clena avip controller block
   FD_CLR (avip_ctrl_client_fd, &avip_ctrl_fdset);
   close (avip_ctrl_client_fd);
   avip_ctrl_client_fd = -1;
   avip_ctrl_max_fd = avip_ctrl_server_fd;
   // wait till stop thread messages are printed
   // just to make sure that messages are printed in order
   sleep(1);
   IXIA_MSG ("AVIP Test Disconnected");
}


void ixia_device::join_threads()
{
   ixia_vhost_server_thread.join();
   avip_data_server_thread.join();
   avip_ctrl_server_thread.join();
}


int ixia_device::add_fd (adaptor_block block, int fd, int type)
{
   switch (block)
   {
      case ixia_block:{
         if (ixia_max_fd >= FD_SETSIZE) {
         IXIA_MSG ("Failed to add new event, fd=%d max_fd=%d FD_SETSIZE=%d", fd, ixia_max_fd, FD_SETSIZE);
         return -1;
         }
         FD_SET(fd, &ixia_fdset);
         if (fd > ixia_max_fd) ixia_max_fd = fd;
         switch (type)
         {            
            case 0: { IXIA_MSG ("Port (%d) Added (fd=%d max=%d)", port_num, fd, ixia_max_fd); break; }
            case 1: { IXIA_MSG ("Vhost Socket Server Started (fd=%d, max=%d)", fd, ixia_max_fd); break; }
         }
         break;
         }

      case avip_data_block:{
         switch (type)
         {
            case 0: {
               int key = 0;
               map <int, port_connection_info*>::iterator i;
               for (i=port.begin(); i!=port.end(); i++) {
                  if (i->second->avip_socket_client_fd == fd) {
                  key = i->first;
                  break;
                  }
               }
               AVIP_MSG ("AVIP Port (%d) Added (fd=%d)", port[key]->port_num, fd);
               break;
               }
            case 1: {
               if (avip_data_max_fd >= FD_SETSIZE) {
               AVIP_MSG ("Failed to add new event, fd=%d max_fd=%d FD_SETSIZE=%d", fd, avip_data_max_fd, FD_SETSIZE);
               return -1;
               }
               FD_SET(fd, &avip_data_fdset);
               if (fd > avip_data_max_fd) avip_data_max_fd = fd;
               AVIP_MSG ("Data Socket Server Started (fd=%d, max=%d)", fd, avip_data_max_fd);
               break;
               }
         }
         break;
         }

      case avip_ctrl_block:{
         if (avip_ctrl_max_fd >= FD_SETSIZE) {
         AVIP_MSG ("Failed to add new Ctrl event");
         return -1;
         }
         FD_SET(fd, &avip_ctrl_fdset);
         if (fd > avip_ctrl_max_fd) avip_ctrl_max_fd = fd;
         switch (type)
         {            
         case 0: {
            IXIA_MSG ("Control Port Added (sock_id=%d, max=%d)", fd, avip_ctrl_max_fd); 
            IXIA_MSG ("AVIP Test Connected");
            break;
            }
         case 1: { AVIP_MSG ("Control Server Started (sock_id=%d, max=%d)", fd, avip_ctrl_max_fd); break; }
         }
         break;
         }
   }
}


ixia_device::ixia_device (string ixia_vlm_path, string avip_data_path, string avip_ctrl_path, string mode, unsigned int poll_interval)
{ 
   reset();
   this->ixia_vlm_path = ixia_vlm_path;
   this->avip_data_path = avip_data_path;
   this->avip_ctrl_path = avip_ctrl_path;
   this->mode = mode;
   this->poll_interval = poll_interval;
}


void ixia_device::reset()
{
   ixia_max_fd           = -1;
   avip_data_max_fd      = -1;
   avip_ctrl_max_fd      = -1;
   ixia_server_fd        = -1;
   avip_data_server_fd   = -1;
   avip_ctrl_server_fd   = -1;
   avip_ctrl_client_fd   = -1;
   ixia_server_addr      = {0};
   avip_data_server_addr = {0};
   avip_ctrl_server_addr = {0};
   FD_ZERO (&ixia_fdset);
   FD_ZERO (&avip_data_fdset);
   FD_ZERO (&avip_ctrl_fdset);
   ixia_vlm_path.clear();
   avip_data_path.clear();
   avip_ctrl_path.clear();
   mode.clear();
   port_num = 0;
   poll_interval = 1;
}


int main (int argc, char *argv[])
{

   // exit if no parameters are passed
   if (argc==1)
      display_usage_and_exit();


   // variables to hold command line parameters
   string ixia_vlm_path;
   string avip_data_path;
   string avip_ctrl_path;
   string log_fname;
   string mode;
   int    port; 
   unsigned int poll_interval;
   int sock_buf_size;


   // default values
   ixia_vlm_path  = "";
   avip_data_path = "";
   avip_ctrl_path = "";
   log_fname      = "ixia_cdn_adaptor.log";
   verbosity      = "low"; 
   mode           = "local";
   port           = 8888; 
   poll_interval  = 1;
   sock_buf_size  = -1;


   // id for command line parameters
   enum {
      opt_ixia_vlm_path  = 1000,
      opt_avip_data_path ,
      opt_avip_ctrl_path ,
      opt_log            ,
      opt_mode           ,
      opt_port           ,
      opt_verbosity      ,
      opt_poll_interval  ,
      opt_sock_buf_size  ,
      opt_help
   };


   // define command line attributes
   static struct option longOptions[] = 
   {
      {"ixia_vlm_path"  , required_argument, 0, opt_ixia_vlm_path  },
      {"avip_data_path" , required_argument, 0, opt_avip_data_path },
      {"avip_ctrl_path" , required_argument, 0, opt_avip_ctrl_path },
      {"log"            , required_argument, 0, opt_log            },
      {"mode"           , required_argument, 0, opt_mode           },
      {"port"           , required_argument, 0, opt_port           },
      {"verbosity"      , required_argument, 0, opt_verbosity      },
      {"poll_interval"  , required_argument, 0, opt_poll_interval  },
      {"sock_buf_size"  , required_argument, 0, opt_sock_buf_size  },
      {"help"           , no_argument      , 0, opt_help           },
      {0, 0, 0, 0}
   };


   // scan commandline arguments
   int opt=0;
   while ((opt = getopt_long_only(argc, argv, "", longOptions, NULL)) != -1)
   {
      switch (opt) 
      {
         case opt_ixia_vlm_path  : { ixia_vlm_path  = optarg; break; }
         case opt_avip_data_path : { avip_data_path = optarg; break; }
         case opt_avip_ctrl_path : { avip_ctrl_path = optarg; break; }
         case opt_log            : { log_fname      = optarg; break; }
         case opt_mode           : { mode           = optarg; break; }
         case opt_verbosity      : { verbosity      = optarg; break; }
         case opt_port           : { port           = atoi(optarg); break; }
         case opt_poll_interval  : { poll_interval  = atoi(optarg); break; }
         case opt_sock_buf_size  : { sock_buf_size  = atoi(optarg); break; }
         case opt_help           : { display_usage_and_exit(); break; }
         case '?'                : { display_usage_and_exit(); break; }
      }
   }


   // exit if we are not able to create a log file
   log_fp = fopen (log_fname.c_str(), "w");
   if (log_fp == NULL) 
   {
      printf ("*** Error: Unable to create log file\n");
      exit(EXIT_FAILURE);
   }


   // set socket buffer size, can be done only as root
   if (sock_buf_size != -1)
   {
      if (geteuid() != 0)
      {
         printf ("*** Error: Changing Socket buffer size requires root permission.\n");
         exit(1);
      }
      ostringstream cmd1, cmd2, cmd3, cmd4;
      cmd1 << "sysctl -w net.core.rmem_default=" << dec << sock_buf_size << " > /dev/null";
      cmd2 << "sysctl -w net.core.wmem_default=" << dec << sock_buf_size << " > /dev/null";
      cmd3 << "sysctl -w net.core.rmem_max="     << dec << sock_buf_size << " > /dev/null";
      cmd4 << "sysctl -w net.core.wmem_max="     << dec << sock_buf_size << " > /dev/null";
      system (cmd1.str().c_str());
      system (cmd2.str().c_str());
      system (cmd3.str().c_str());
      system (cmd4.str().c_str());
   }


   // print banner
   IXIA_MSGNOF ("--------------------------------------------------------------------------------\n");
   IXIA_MSGNOF ("Cadence Accelerated VIP\n");
   IXIA_MSGNOF ("(c) Cadence Design Systems, Inc. All rights reserved\n");
   IXIA_MSGNOF ("Ixia Avip Adaptor (v-%llu) \n", version);
   IXIA_MSGNOF ("--------------------------------------------------------------------------------\n");
   IXIA_MSGNOF ("Ixia-AVIP Connection mode    : %s\n",    mode.c_str()           );
   IXIA_MSGNOF ("Ixia Socket                  : %s\n",    ixia_vlm_path.c_str()  );
   IXIA_MSGNOF ("Avip Socket (data)           : %s\n",    avip_data_path.c_str() );
   IXIA_MSGNOF ("Avip Socket (ctrl)           : %s\n",    avip_ctrl_path.c_str() );
   IXIA_MSGNOF ("Max File Descriptors allowed : %d\n",    FD_SETSIZE             );
   IXIA_MSGNOF ("Log filename                 : %s\n",    log_fname.c_str()      );
   IXIA_MSGNOF ("Polling Interval             : %d us\n", poll_interval          );
   IXIA_MSGNOF ("Socket Buffer Size           : %d\n",    sock_buf_size          );
   IXIA_MSGNOF ("--------------------------------------------------------------------------------\n");


   ixia_device device (
         ixia_vlm_path, 
         avip_data_path, 
         avip_ctrl_path, 
         mode,
         poll_interval
         );


   device.init_ixia_server();
   device.init_avip_ctrl_server();
   device.init_avip_data_server();


   device.start_ixia_server();
   device.start_avip_ctrl_server();
   device.start_avip_data_server();
   device.join_threads();


   return 0;
}
