ifeq "${DBG}" "on"
VERBOSITY=2
else
VERBOSITY=1
endif

default:
	@rm -f mpktgen; \
	gcc mpktgen.c -lpthread -lm -o mpktgen; \
	./mpktgen 				\
		-frm_size=100 		\
		-frm_cnt=10   		\
		-txbuffer_size=64 	\
		-rxbuffer_size=128 	\
		-verbosity=${VERBOSITY}

c:
	@gcc mpktgen.c -lpthread -o mpktgen

g:
	@gcc gui.c -o gui; ./gui

a:
	@g++ avip.cpp -lpthread -std=c++11 -o avip; ./avip

clean:
	@rm -rf *.log *.out mpktgen gui avip
