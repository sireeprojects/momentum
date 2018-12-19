#include "systemc.h"
#include "common.h"


int sc_main (int argc, char *argv[])
{
    open_logfile();
    
    // time precision 1ps is implicit
    sc_set_time_resolution(1, SC_PS);
    
    // Clock generation
    sc_time freq (5, SC_NS);
    sc_clock clk ("clk", freq);
    
    bool en_waves = false;
    sc_trace_file *waves;
    
    // Module interconnects
    sc_uint<8> data;
    
    data = 0xff;
    MSG ("Data= " << hex << data);
    
    data(1,0) = 0;
    MSG ("Data= " << hex << data);
    
    if (en_waves) {
        waves = sc_create_vcd_trace_file ("waves");
        // wdump (clock);
    }
    
    // Start simulation
    sc_start (2000, SC_NS);
    
    // Close waveform file
    if (en_waves)
        sc_close_vcd_trace_file(waves);
    
    close_logfile();
    
    return 0;
}
