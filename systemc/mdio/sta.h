#include "systemc.h"


using namespace std;


SC_MODULE(sta) {
    sc_in   <bool>         clk;
    sc_out  <bool>         rst;
    sc_out  <bool>         en;
    sc_out  <sc_uint< 2> > op_code;
    sc_out  <sc_uint< 5> > phy_addr;
    sc_out  <sc_uint< 5> > reg_addr;
    sc_out  <sc_uint<16> > data;
    sc_out  <bool>         clause;   
    sc_in   <bool>         done;   
    
    // Constructor
    SC_HAS_PROCESS(sta);
    sta(sc_module_name name);
    
    // Functions
    void reset();
    void over();
    void test();
    void gen_reset();
    void start_of_simulation();
    const char* id() {return (const char *)name();}
    friend void sc_trace(sc_trace_file*& tf, const sta& obj, string name);
    
    // Variables
    int nof_cycles;
    int rst_interval;
    sc_event rst_done;
    sc_event mdio_done;
};


void sta::test() {
    sc_time cycle(5, SC_NS);
    
    wait(rst_done);
    
    MSG("Driving stimulus");
    // stimulus
    clause   = clause22;
    op_code  = op_write;
    phy_addr = 0x5;
    reg_addr = 0x1;
    data     = 0x5555;
    wait (cycle);
    en = 1;
    wait (cycle);
    en = 0;
    
    wait(mdio_done);
    wait(100, SC_NS);
    MSG("Driving stimulus");
    // stimulus
    clause   = clause22;
    op_code  = op_write;
    phy_addr = 0x3;
    reg_addr = 0x5;
    data     = 0xcccc;
    wait(cycle);
    en = 1;
    wait(cycle);
    en = 0;
    
    wait(mdio_done);
    wait(100, SC_NS);
    // MSG("Driving stimulus");
    // third stimulus
    clause   = clause22;
    op_code  = op_write;
    phy_addr = 0x1A;
    reg_addr = 0x8;
    data     = 0xeeee;
    wait(cycle);
    en = 1;
    wait(cycle);
    en = 0;
}


// default values of all variables
void sta::reset() {
    rst      = 1;
    op_code  = 0;
    phy_addr = 0;
    reg_addr = 0;
    data     = 0;
    clause   = 0;
    nof_cycles = 0;
    rst_interval = 10;
}


void sta::over() {
    mdio_done.notify();
}


void sta::gen_reset() {
    if (nof_cycles >= rst_interval) {
        rst.write(0);
        rst_done.notify();
    }
    else
        nof_cycles++;
}


void sc_trace (sc_trace_file*& tf, const sta& obj, string name) {
    sc_trace (tf, obj.clk      , name + ".clk     ");
    sc_trace (tf, obj.rst      , name + ".rst     ");
    sc_trace (tf, obj.op_code  , name + ".op_code ");
    sc_trace (tf, obj.phy_addr , name + ".phy_addr");
    sc_trace (tf, obj.reg_addr , name + ".reg_addr");
    sc_trace (tf, obj.data     , name + ".data    ");
    sc_trace (tf, obj.clause   , name + ".clause  ");
}


sta::sta (sc_module_name name):sc_module(name) {
    SC_THREAD (test);
    
    SC_METHOD (gen_reset);
    sensitive << clk.pos();
    
    SC_METHOD (over);
    sensitive << done.pos();
}


void sta::start_of_simulation() {
    reset();
}
