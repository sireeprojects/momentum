SCHOME=/sw/systemc

run:compile
	@./main.x


compile:clean
	@g++ \
	   -I${SCHOME}/include \
	   -lsystemc \
	   -L${SCHOME}/lib-linux64 \
	   main.cpp \
	   -o main.x
	

clean:
	@rm -rf waves.vcd main.x *.log *.diag
