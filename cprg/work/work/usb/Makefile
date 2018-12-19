#path to libusb installation
LIBUSB=/usr

LIBUSB_INCL=${LIBUSB}/include/libusb-1.0
LIBUSB_LIBS=${LIBUSB}/lib

all: clean usb avip

usb:
	@g++                 \
		-std=c++0x        \
		-pthread          \
		-lusb-1.0         \
      usboeSocket.cpp   \
      usb_mon.cpp       \
      -I${LIBUSB_INCL}  \
      -L${LIBUSB_LIBS}  \
      -o start2_monitor_usb_dev

avip:
	@g++                   \
		-std=c++0x          \
		-pthread            \
		-lusb-1.0           \
      usboeSocket.cpp     \
      avip_test.cpp       \
      -I${LIBUSB_INCL}    \
      -L${LIBUSB_LIBS}    \
      -o start1_avip_test

clean:
	@rm -rf start1_avip_test start2_monitor_usb_dev mon_ixia

ixia:
	@g++                   \
		-std=c++0x          \
		-pthread            \
      usboeSocket.cpp     \
      mon_ixia.cpp        \
      -o mon_ixia
