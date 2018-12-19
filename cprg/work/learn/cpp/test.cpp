#include "avip_pcap.h"

// Test program
int main () 
{
   unsigned char *pkt_data = new unsigned char [200];

   // prepare file for pcie
   avip_pcap pcie (AVIP_PCIE, 10000); // 10000=max len of a packet
   pcie.pcap_fopen ("pcie.pcap", AVIP_WR);

   // pkt_data = pcie.pcap_fread_next ();
   // pkt_data = pcie.pcap_fread_next ();
   // pkt_data = pcie.pcap_fread_next ();
   // pcie.pcap_fclose();

/*
//==================
// Frame #1, Write a single frame instantly

   // populate dummy payload
   for (int i=0; i<200; i++) pkt_data [i]= (unsigned char) i;
   pcie.pcap_fwrite (200, pkt_data); // 200=len of current pkt, pkt_data=pointer to data array


//==================
// Frame #2, write a 150 byte frames in parts =======

   // prepare data
   unsigned char *pkt_data_f1 = new unsigned char [50];
   unsigned char *pkt_data_f2 = new unsigned char [50];
   unsigned char *pkt_data_f3 = new unsigned char [50];

   // populate dummy payload
   for (int i=0; i<50; i++) pkt_data_f1 [i]= (unsigned char) 1;
   for (int i=0; i<50; i++) pkt_data_f2 [i]= (unsigned char) 2;
   for (int i=0; i<50; i++) pkt_data_f3 [i]= (unsigned char) 3;

   // write a frame in parts
   pcie.pcap_append (50, pkt_data_f1); // will not write to file yet
   pcie.pcap_append (50, pkt_data_f2); // will not write to file yet
   pcie.pcap_append (50, pkt_data_f3); // will not write to file yet
   pcie.pcap_fwrite(); // write the complete frame into file


//==================
// Frame #3, write a 600 byte frames in parts =======

   // prepare data
   unsigned char *pkt_data_ff1 = new unsigned char [200];
   unsigned char *pkt_data_ff2 = new unsigned char [200];
   unsigned char *pkt_data_ff3 = new unsigned char [200];

   // populate dummy payload
   for (int i=0; i<200; i++) pkt_data_ff1 [i]= (unsigned char) 4;
   for (int i=0; i<200; i++) pkt_data_ff2 [i]= (unsigned char) 5;
   for (int i=0; i<200; i++) pkt_data_ff3 [i]= (unsigned char) 6;

   // write a frame in parts
   pcie.pcap_append (200, pkt_data_ff1); // will not write to file yet
   pcie.pcap_append (200, pkt_data_ff2); // will not write to file yet
   pcie.pcap_append (200, pkt_data_ff3); // will not write to file yet
   pcie.pcap_fwrite(); // write the complete frame into file

   pcie.pcap_fclose();
*/
   return 0;
};
