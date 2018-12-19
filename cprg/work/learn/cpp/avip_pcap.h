#ifdef AVIP_PCAP
#else
#define AVIP_PCAP

// Header only implementation

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iomanip>

using namespace std;

// TODO: implement error handling

// TODO: assign proper enum values to match link type
// in global header for every avip
typedef enum {
   AVIP_ENET=1,
   AVIP_PCIE=2,
   AVIP_AXI=3
   } avip_name;


typedef enum {
   AVIP_RD=1,
   AVIP_WR=2
   } avip_rw;


struct __attribute__((__packed__)) 
avip_pcap_global_hdr {
	unsigned int magic           : 32;
	unsigned short version_major : 16;
	unsigned short version_minor : 16;
	unsigned int thiszone        : 32;
	unsigned int sigfigs         : 32;
	unsigned int snaplen         : 32;
	unsigned int linktype        : 32;
   };


struct __attribute__((__packed__)) 
avip_pcap_packet_hdr {
   unsigned int tv_sec  : 32;
   unsigned int tv_usec : 32;
	unsigned int caplen  : 32;
	unsigned int len     : 32;
   };
   

class avip_pcap 
{


public: // User API's


      unsigned int pcap_fopen (string fname, avip_rw mode) 
      {
         switch (mode) {
         case AVIP_RD: {fp = fopen(fname.c_str(),"r"); break;}
         case AVIP_WR: {fp = fopen(fname.c_str(),"w"); break;}
         default: {
            // throw error message
            }
         }
         return 0;
      }


      unsigned int pcap_fclose() 
      {
         fclose (fp);
         return 0;
      }


      unsigned int pcap_fwrite(unsigned int pkt_len, unsigned char *data) 
      {
         switch (gh_write_done) {
            case false: {
               // Write Pcap Global Header
               gh_w.magic         = 0xa1b2c3d4;
	            gh_w.version_major = 2;
	            gh_w.version_minor = 4;
	            gh_w.thiszone      = 0;
	            gh_w.sigfigs       = 0;
	            gh_w.snaplen       = 4194304;
	            gh_w.linktype      = 1; // FIXME when plugin is ready we can use 'name'
	            // gh_w.linktype      = name;
               
               // Write Global Header to file
               fwrite(&gh_w, 1, sizeof(avip_pcap_global_hdr), fp);

               // Write Pcap Packet Header
               ph_w.tv_sec    = 0;
               ph_w.tv_usec   = 0;
               ph_w.caplen    = pkt_len;
               ph_w.len       = pkt_len;
               /*write Pcap Packet Header to file*/
               fwrite(&ph_w, 1, sizeof(avip_pcap_packet_hdr), fp);
               // write Packet Data to file
               fwrite(data,1,pkt_len, fp); 

               gh_write_done = true;
               break;
               }
            case true: {
               // Write Pcap Packet Header
               ph_w.tv_sec    = 0;
               ph_w.tv_usec   = 0;
               ph_w.caplen    = pkt_len;
               ph_w.len       = pkt_len;
               /*write Pcap Packet Header to file*/
               fwrite(&ph_w, 1, sizeof(avip_pcap_packet_hdr), fp);
               // write Packet Data to file
               fwrite(data,1,pkt_len, fp); 
               break;
               }
         }
         return 0;
      }

      // overloaded, final write after multiple appends
      unsigned int pcap_fwrite() 
      {
         pcap_fwrite (plen, pdw);
         plen = 0;
         return 0;
      }

      // to enable pcap write in parts
      // this will not write to the file
      // have to use pcap_fwrite() to do the final write
      unsigned int pcap_append(unsigned int pkt_len, unsigned char *data)
      {
         memcpy (pdw+plen, data, pkt_len);
         plen += pkt_len;
         return 0;
      }

      unsigned char * pcap_fread_next () 
      {
         switch (gh_read_done) {
            case false: {
               // read the global header
               fread((unsigned char*)&gh_r, sizeof(avip_pcap_global_hdr), 1, fp);
               print_gh (&gh_r);
               gh_read_done = true;

               // read the first packet header
               fread((char*)&ph_r, 16, 1, fp);
               print_ph (&ph_r);

               // read the first packet data
               fread((char*)pdr, ph_r.len, 1, fp);
               print_data (pdr, ph_r.len);
               break;
               }
            case true: {
               // read the next packet header
               fread((char*)&ph_r, 16, 1, fp);
               print_ph (&ph_r);

               // read the next packet data
               fread((char*)pdr, ph_r.len, 1, fp);
               print_data (pdr, ph_r.len);
               // read the next packet header and payload data
               break;
               }
         }
      }


      // CTOR
      avip_pcap(avip_name n, unsigned int max_len) 
      {
         name = n;
         pdw = new unsigned char [max_len];
         pdr = new char [max_len];
         plen = 0;
         gh_read_done = false;
         gh_write_done = false;
      }


      // DTOR
      ~avip_pcap() 
      {
         delete [] pdw;
      }

private:
      avip_name name;   
      avip_rw fmode;
      unsigned char * pdw;
      char * pdr;
      unsigned int plen;
      FILE * fp;
      avip_pcap_global_hdr gh_w;
      avip_pcap_packet_hdr ph_w;

      avip_pcap_global_hdr gh_r;
      avip_pcap_packet_hdr ph_r;
      bool gh_read_done;
      bool gh_write_done;

      void print_ph (avip_pcap_packet_hdr * pkh) 
      {
         cout << "tv_sec  = " << hex << pkh->tv_sec  << endl;
         cout << "tv_usec = " << hex << pkh->tv_usec << endl;
	      cout << "caplen  = " << dec << pkh->caplen  << endl;
	      cout << "len     = " << dec << pkh->len     << endl;
      }

      void print_gh (avip_pcap_global_hdr * glh) 
      {
	      cout << "magic         = " << hex << glh->magic         << endl;
	      cout << "version_major = " << hex << glh->version_major << endl;
	      cout << "version_minor = " << hex << glh->version_minor << endl;
	      cout << "thiszone      = " << hex << glh->thiszone      << endl;
	      cout << "sigfigs       = " << hex << glh->sigfigs       << endl;
	      cout << "snaplen       = " << hex << glh->snaplen       << endl;
	      cout << "linktype      = " << hex << glh->linktype      << endl;
      }

      void print_data (char * d, unsigned int len) 
      {
         for (int i=0; i<len ; i++) 
         {
            cout << setw(2) << setfill ('0') << hex << (0x00ff & ((short) d[i]))<< " ";
            if (i%25==0) cout << endl;
         }
         cout << endl;
      }
};

#endif
