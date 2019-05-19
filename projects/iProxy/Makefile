libtb:
	g++ iProxy.cpp -s -fPIC -shared -o libiProxy.so -Wall -Wno-unused -lpthread

compile:
	g++ main.cpp -liProxy -o test.x -L/mnt/hgfs/vmshare/iProxy


run:libtb compile
	./test.x

clean:
	rm -rf *.so run.log *.x


git: clean	
	gitall
