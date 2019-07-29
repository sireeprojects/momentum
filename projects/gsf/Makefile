SVINC=/work/tools/msim-6.5/modeltech/include

default:clean lib comp run

lib:
	@vlib work; vmap work work

dotso:
	@gcc -I. -I${SVINC} -shared -fPIC -o gsf.so sw_top.c	

comp:dotso
	@vlog -sv hw_top.sv -nologo

run:
	@vsim \
		hw_top \
		-quiet -c -wlfdeleteonquit \
		-sv_lib gsf \
		-do "run -all;exit" \
		-l "run.log"

clean:
	@rm -rf work transcript *.wlf *.vcd *.so *.log
