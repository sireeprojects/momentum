RESOURCE=/root/work/momentum/projects/lib


default: clean
	@gcc server.c -o server.x -I${RESOURCE};\
	gcc client.c -o client.x -I${RESOURCE}

clean:
	@rm -rf *.x run.log
