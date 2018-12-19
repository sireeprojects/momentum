vlog bitw.v
vsim -c top
run -All


# vlog hello.c hello.sv
# vsim -c hello
# vcd file waves.vcd
# vcd add hello/*
# run -All


#vlog hello.sv hello.c
#vsim -c hello -do "run -all;quit"
