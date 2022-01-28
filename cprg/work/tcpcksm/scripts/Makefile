TCPLIB=/root/tcpcksm

run:link
	@gcc \
		-o test 	\
		test.c	\
		-ltcplib \
		-I${TCPLIB}/src \
		-L${TCPLIB}/lib ; ./test

link:compile
	@gcc -shared -o ${TCPLIB}/lib/libtcplib.so ${TCPLIB}/lib/tcplib.o

compile:
	@gcc \
		-c -Wall 	\
		-Werror 		\
		-fpic 		\
		-I${TCPLIB}/src 	\
		${TCPLIB}/src/tcplib.c \
		-o ${TCPLIB}/lib/tcplib.o

clean:
	@rm -rf ${TCPLIB}/lib/* ${PWD}/test

