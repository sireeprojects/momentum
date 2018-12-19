run:compile
	@./main.x


compile:clean
	@g++ \
	   -I/sw/systemc/include \
	   -lsystemc \
	   -L/sw/systemc/lib-linux64 \
	   main.cpp \
	   -o main.x
	

clean:
	@rm -rf waves.vcd main.x *.log *.diag
