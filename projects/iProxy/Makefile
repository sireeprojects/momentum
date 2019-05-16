libtb:
	g++ iProxy.cpp -s -fPIC -shared -o libiProxy.so -Wall -Wno-unused

compile:
	g++ test.cpp -liProxy -o test.x -L/root/momentum/projects/iProxy


run:libtb compile
	./test.x

clean:
	rm -rf *.so run.log *.x


git: clean	
	gitall
