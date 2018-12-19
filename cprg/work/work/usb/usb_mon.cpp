#include <iostream>
#include <libusb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "usboeSocket.h"

using namespace std;

int main()
{
   usboeSocket sock;
   unsigned int VID = 0x413c;
   unsigned int PID = 0x2113;
   unsigned int EP = 1;
   unsigned int DL = 8;

   int r; 
   int actual; 
   ssize_t cnt; 
   unsigned char data2[DL];
   unsigned char *data = new unsigned char[DL]; 
   
   libusb_device **devs; 
   libusb_device_handle *dev_handle; 
   libusb_context *ctx = NULL; 

   sock.connect_to_server((char*)"10.253.64.8", 8888);
   
   r = libusb_init(&ctx); 
   if(r < 0) 
   {
      cout<<"Init Error "<<r<<endl; 
      return 1;
   }
   libusb_set_debug(ctx, 3); 
   
   cnt = libusb_get_device_list(ctx, &devs); 
   if(cnt < 0) 
   {
      cout<<"Get Device Error"<<endl; 
      return 1;
   }
   
   dev_handle = libusb_open_device_with_vid_pid(ctx, VID, PID); 
   if(dev_handle == NULL)
      cout<<"Cannot open device"<<endl;
   else
      cout<<">> USB Device Opened"<<endl;
   libusb_free_device_list(devs, 1); 
   
   if(libusb_kernel_driver_active(dev_handle, 0) == 1) 
   {
      cout<<">> Kernel Driver Active"<<endl;
      if(libusb_detach_kernel_driver(dev_handle, 0) == 0) 
      cout<<">> Kernel Driver Detached!"<<endl;
   }
   r = libusb_claim_interface(dev_handle, 0); 

   if(r < 0) 
   {
      cout<<">> Cannot Claim USB Interface"<<endl;
      return 1;
   }
   cout<<">> USB  Interface Claimed"<<endl;
   
   while (1)
   {
      r = libusb_interrupt_transfer(dev_handle, (EP | LIBUSB_ENDPOINT_IN), data, DL, &actual, 0); 
      if(r == 0 && actual == DL) 
      {
         printf ("Data Sent to AVIP -> ");
      	for (int i=0 ;i < DL; i++) 
      	   printf("%02x;",data[i]);
         printf("\n");

         sock.send_data ((char*)data, DL);
         if (data[2]==0x29)
            goto close;
      }
      else 
      {
      	cout << "Cannot complete the transfer due to error - " << libusb_error_name(r) << endl;
         cout<<"Error"<<endl;
      }
   } // while
   
close:   
   r = libusb_release_interface(dev_handle, 0); 
   if(r!=0) 
      return 1;
   cout << "Released Interface" <<endl;

   printf ("Closing Socket connection\n");
   sock.close_client ();
   
   libusb_close(dev_handle); 
   libusb_exit(ctx); 
   return 0;
}
