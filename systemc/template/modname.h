

SC_MODULE (modname)
{
   // IO ports
   sc_in<bool> clk;

   // Constructor
   SC_CTOR (modname) {
      SC_THREAD (main);
         sensitive << clk.pos();
         dont_initialize();

      initial();
   }

   // default values of all variables
   void initial() 
   {
   }
   
   // main thread
   void main() 
   {
      wait();
   }
};
