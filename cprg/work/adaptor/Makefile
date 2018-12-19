default:clean version
	@g++ 								\
      -I. -s -m64 -std=c++0x  \
		-I${PWD}/atomic         \
		-lpthread               \
      -o ixia_cdn_adaptor 		\
      adaptor.cpp ; 				\
		rm -f version.h genver

version:
	@g++ 								\
      -I. -s -m64 				\
      version.cpp -o genver; 	\
		./genver

clean:
	@rm -f ixia_cdn_adaptor *.o ixia_cdn_adaptor.log version.h genver
