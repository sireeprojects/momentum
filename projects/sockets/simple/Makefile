RESOURCE=/root/momentum/projects/lib


tcp:clean
	@gcc client.c -o client -I${RESOURCE}


default: clean
	@gcc server.c -o server -I${RESOURCE};\
	gcc client.c -o client -I${RESOURCE}


clean:
	@rm -rf run.log client server
