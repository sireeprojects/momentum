#include "systemc.h"


using namespace std;


SC_MODULE(mdio_master) {
    sc_in  <bool>         clk;
    sc_in  <bool>         rst;
    sc_in  <bool>         en;
    sc_in  <sc_uint<2>  > op_code;
    sc_in  <sc_uint<5>  > phy_addr;
    sc_in  <sc_uint<5>  > reg_addr;
    sc_in  <sc_uint<16> > data;
    sc_in  <bool>         clause;
    sc_out <sc_logic>     mdio;
    sc_out <bool>         mdc;
    sc_out <bool>         done;
    
    // Constructor
    SC_HAS_PROCESS(mdio_master);
    mdio_master(sc_module_name name);
    
    // Functions
    void main();
    void start();
    void bitreverse();
    
    // Variables
    sc_lv<64> data_to_shift;
    bool en_mdio;
    sc_int<2> state;
    int shift_count;
    
    sc_clock divclock;
    
    void reset();
    void start_of_simulation();
    const char* id() {return (const char *) name();}
    friend void sc_trace(sc_trace_file*& tf, const mdio_master& obj, string name);
};


// default values of all variables
void mdio_master::reset() {
    data_to_shift = 0;
    en_mdio = false;
    state = IDLE;
    mdc = 0;
    mdio = sc_logic('0');
    shift_count = 0;
    done = 0;
}


void mdio_master::start() {
    state = IDLE;
    shift_count = 0;
    en_mdio = true;
}


void mdio_master::bitreverse() {
    data_to_shift.range(31, 0) = data_to_shift.range( 0,31);
    data_to_shift.range(33,32) = data_to_shift.range(32,33);
    data_to_shift.range(35,34) = data_to_shift.range(34,35);
    data_to_shift.range(40,36) = data_to_shift.range(36,40);
    data_to_shift.range(45,41) = data_to_shift.range(41,45);
    data_to_shift.range(47,46) = data_to_shift.range(46,47);
    data_to_shift.range(63,48) = data_to_shift.range(48,63);
}


// main always @(posedge clk) 
// with syncronous reset
void mdio_master::main() {
    if (rst.read()) {
        reset();
    }
    else {
        if (en_mdio) {
            switch (state) {
                case IDLE:{
                    switch (clause.read()) {
                        case clause45:{
                            break;
                        }
                        case clause22:{
                            done = 0;
                            data_to_shift.range(31, 0) = 0xffffffff;
                            data_to_shift.range(33,32) = clause22;
                            data_to_shift.range(35,34) = op_code;
                            data_to_shift.range(40,36) = phy_addr;
                            data_to_shift.range(45,41) = reg_addr;
                            data_to_shift.range(47,46) = "zz";
                            data_to_shift.range(63,48) = data;
                            bitreverse();
                            break;
                        }
                    }
                    state = SHIFT_OUT;
                    MSG("Starting MDIO Op in next cycle");
                    break;
                    }
                case SHIFT_OUT:{
                    mdio = data_to_shift[0];
                    data_to_shift = data_to_shift >> 1;
                    shift_count++;
                    if (shift_count>=65) {
                        en_mdio = false;
                        done = 1;
                        state = SHIFT_DONE;
                    }
                    break;
                    }
                case SHIFT_DONE:{
                    state = IDLE;
                    break;
                    }
                default:{
                    break;
                }
            } // switch
        } // en_mdio
        else {
            done = 0;
            mdio = sc_logic('0');;
            mdc = 0;
        }
    }
}


mdio_master::mdio_master(sc_module_name name):sc_module(name), divclock("divclock", 5, SC_NS) {
    SC_METHOD(main);
        sensitive << divclock.posedge_event();
        dont_initialize();
    
    SC_METHOD(start);
        sensitive << en.pos();
        dont_initialize();
}


void sc_trace(sc_trace_file*& tf, const mdio_master& obj, string name) {
    sc_trace(tf, obj.clk      , name + ".clk     ");
    sc_trace(tf, obj.rst      , name + ".rst     ");
    sc_trace(tf, obj.op_code  , name + ".op_code ");
    sc_trace(tf, obj.phy_addr , name + ".phy_addr");
    sc_trace(tf, obj.reg_addr , name + ".reg_addr");
    sc_trace(tf, obj.data     , name + ".data    ");
    sc_trace(tf, obj.clause   , name + ".clause  ");
    sc_trace(tf, obj.mdio     , name + ".mdio    ");
    sc_trace(tf, obj.mdc      , name + ".mdc     ");
    sc_trace(tf, obj.en       , name + ".en      ");
    sc_trace(tf, obj.en_mdio  , name + ".en_mdio ");
    sc_trace(tf, obj.done     , name + ".done    ");
    sc_trace(tf, obj.state    , name + ".state   ");
}
   

void mdio_master::start_of_simulation() {
    reset();
}
