ifeq "${DBG}" "on"
VERBOSITY=2
else
VERBOSITY=1
endif

default:
	@gcc 					\
	mpktgen.c 				\
	-lpthread 				\
	-o mpktgen; 			\
	./mpktgen 				\
		-frm_size=100 		\
		-frm_cnt=200 		\
		-txbuffer_size=64 	\
		-rxbuffer_size=128 	\
		-verbosity=${VERBOSITY}

c:clean
	@gcc 		\
	mpktgen.c 	\
	-lpthread 	\
	-o mpktgen

r:
	@./mpktgen				\
		-frm_size=100 		\
		-frm_cnt=200 		\
		-txbuffer_size=64 	\
		-rxbuffer_size=128 	\
		-verbosity=${VERBOSITY}

clean:
	@rm -rf *.log mpktgen
