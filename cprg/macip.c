#include <stdio.h>
#define MAC_ADDR_BYTES 6
#define IP_ADDR_BYTES  4
#define MAX_U8_VAL         0xFF

void incr_ip_mac_addr(unsigned char *pAddr, unsigned int *i, unsigned int *p, unsigned int max_val)
{
 unsigned int incr_index, parent_index ;
 incr_index = *i ;
 parent_index = *p ;
 if ( pAddr[incr_index] == max_val ){
 if ( pAddr[parent_index] == max_val) {
 pAddr[parent_index] = 0 ;
 parent_index--;
 }
 pAddr[parent_index] +=1 ;
 pAddr[incr_index] = 0 ;
 }else{
 pAddr[incr_index]++ ;
 }

 *i = incr_index ;
 *p = parent_index ;
}
int main(void)
{
 unsigned int i;
 unsigned int mac_incr_idx = MAC_ADDR_BYTES - 1, mac_parent_idx = MAC_ADDR_BYTES - 2 ;
 unsigned int ip_incr_idx = IP_ADDR_BYTES - 1, ip_parent_idx = IP_ADDR_BYTES - 2 ;
 unsigned char mac_addr[MAC_ADDR_BYTES], ip_addr[IP_ADDR_BYTES] ;

 unsigned char init_mac_addr[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 } ;
 unsigned char init_ip_addr[] = { 100, 1, 1, 2 } ;

 memcpy( &mac_addr[0], &init_mac_addr, sizeof(unsigned char)*MAC_ADDR_BYTES) ;
 memcpy( &ip_addr[0], &init_ip_addr, sizeof(unsigned char)*IP_ADDR_BYTES) ;
 for(i=0; i<4096; i++)
 {
 // printf("\n %d.%d.%d.%d", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
 printf("\n%02x:%02x:%02x:%02x:%02x:%02x ", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
 // incr_ip_mac_addr( ip_addr, &ip_incr_idx, &ip_parent_idx, MAX_U8_VAL) ;
 incr_ip_mac_addr( mac_addr, &mac_incr_idx, &mac_parent_idx, MAX_U8_VAL) ;
 }
}

