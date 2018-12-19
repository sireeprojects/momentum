#include "systemc.h"
#include "modname.h"

#define wdump(signal) sc_trace(waves, signal, #signal);

int sc_main(int argc, char *argv[])
{
   // time precision 1ps is implicit
   sc_set_time_resolution(1, SC_PS);

   // Clock generation
   sc_time freq(5, SC_NS);
   sc_clock clock("clk", freq);

   // Module interconnects
   sc_signal<bool> clk;

   // Module Instantiation
   modname modname("modname");

   // Port binding
   modname.clk(clock);

   // Create waveform file
   sc_trace_file *waves;
   waves = sc_create_vcd_trace_file("waves");
   waves->set_time_unit(1, SC_PS); // required ?

   // Probe following signals into waveform
   wdump(clock);

   // Start simulation
   sc_start(1000, SC_NS);

   // Close waveform file
   sc_close_vcd_trace_file(waves);

   return 0;
}
