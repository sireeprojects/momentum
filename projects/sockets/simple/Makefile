RESOURCE=/root/momentum/projects/lib


tcp:clean
	@gcc client.c -o client.x -I${RESOURCE}


default: clean
	@gcc server.c -o server.x -I${RESOURCE};\
	gcc client.c -o client.x -I${RESOURCE}


clean:
	@rm -rf *.x run.log
