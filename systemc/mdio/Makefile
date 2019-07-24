SCHOME=/work/tools/systemc-2.3.3

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
