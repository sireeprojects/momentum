#include "systemc.h"
#include <fstream>
#include <string>
#include <sstream>
#include "common.h"
#include "types.h"
#include "mdio.h"
#include "sta.h"


int sc_main (int argc, char *argv[]) {

    open_logfile();
    
    // time precision 1ps is implicit
    sc_set_time_resolution (1, SC_PS);
    
    // Clock generation
    sc_time freq (5, SC_NS);
    sc_clock clk ("clk", freq);
    
    bool en_waves = true;
    sc_trace_file *waves;
    
    MSG ("Simple message " << en_waves);
    
    // Module interconnects
    sc_signal<bool>         rst;
    sc_signal<bool>         en;
    sc_signal<sc_uint<2> >  op_code;
    sc_signal<sc_uint<5> >  phy_addr;
    sc_signal<sc_uint<5> >  reg_addr;
    sc_signal<sc_uint<16> > data;
    sc_signal<bool>         clause;
    sc_signal<sc_logic>     mdio;
    sc_signal<bool>         mdc;
    sc_signal<bool>         done;
    
    // Module Instantiation
    mdio_master mdio_master ("mdio");
        mdio_master.clk      ( clk      );
        mdio_master.rst      ( rst      );
        mdio_master.en       ( en       );
        mdio_master.op_code  ( op_code  );
        mdio_master.phy_addr ( phy_addr );
        mdio_master.reg_addr ( reg_addr );
        mdio_master.data     ( data     );
        mdio_master.clause   ( clause   );
        mdio_master.mdio     ( mdio     );
        mdio_master.mdc      ( mdc      );
        mdio_master.done     ( done     );
    
    // Station Management Entity
    sta sta ("sta");
        sta.clk      ( clk      );
        sta.rst      ( rst      );
        sta.en       ( en       );
        sta.op_code  ( op_code  );
        sta.phy_addr ( phy_addr );
        sta.reg_addr ( reg_addr );
        sta.data     ( data     );
        sta.clause   ( clause   );
        sta.done     ( done     );
    
    if (en_waves) {
        waves = sc_create_vcd_trace_file ("waves");
        wdump(clock);
        wdump(mdio_master);
        wdump(sta);
    }
    
    // Start simulation
    sc_start(2000, SC_NS);
    
    // Close waveform file
    if (en_waves) 
        sc_close_vcd_trace_file (waves);
    
    close_logfile();
    
    return 0;
}
