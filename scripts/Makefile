PROJECT=/root/work/momentum


run:compile_test
	./test

compile_test:make_momentum_lib
	@gcc \
		-o test.x 					\
		${PROJECT}/run/test.c 	\
		-lmomentum 					\
		-I${PROJECT}/src 			\
		-I${PROJECT}/lib

make_momentum_lib:make_momentum_obj
	@gcc \
		-shared \
		-o ${PROJECT}/lib/libmomentum.so \
		${PROJECT}/lib/libmomentum.o

make_momentum_obj:
	@gcc \
		-c -Wall -Werror \
		-fpic \
		-I${PROJECT}/src \

clean:
	@rm -rf \
		${PROJECT}/lib/* \
		${PROJECT}/test
