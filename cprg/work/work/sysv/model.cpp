#include "systemc.h"

class model : public sc_module 
{
public: 
   // IO ports
   sc_in<sc_logic>  in;
   sc_out<sc_logic> out;

   SC_HAS_PROCESS (model);
   model (sc_module_name name);

   ~model() {}
   void run();
};

model::model (sc_module_name name):
   sc_module (name),
   in  ("in"),
   out ("out")
   {
      SC_METHOD (run);
      sensitive << in;
}

// loopback
void model::run()
{
   out.write(in.read());
}

NCSC_MODULE_EXPORT(model)


// SC_CTOR(model) : in("in"), out("out")
// {
//    SC_METHOD(run);
//       sensitive << in;
// }
