vlib work
vmap work work
/usr/bin/gcc -I/sw/msim/modeltech/include -shared -fPIC -o hello.so hello.c
vlog -sv hello.sv
vsim -c hello -sv_lib hello
run -All
