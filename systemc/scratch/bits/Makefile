run:compile
	@./main.x

compile:clean
	@g++ \
	   -I/sw/systemc/include \
	   -lsystemc \
	   -L/sw/systemc/lib-linux64 \
	   main.cpp \
	   -o main.x
	
waves:
	@simvisdbutil -shm -compress -quiet -overwrite waves.vcd

clean:
	@rm -rf waves.vcd main.x waves.shm .simvision simvisdbutil.log
