`timescale 1ns/1ns

module tb;

   reg    in;
   wire   out;
   
   initial 
   begin
      in = 1'b0;
      #10 in = 1'b1;
      #10 in = 1'bx;
      #10 in = 1'bz;
      #10 in = 1'b0;
      #10 in = 1'b1;
      #10 in = 1'bx;
      #10 in = 1'bz;
      #10 $finish;
   end
   
   // instantiate systemc shell
   model test (in, out);
   
   initial 
   begin
      $shm_open ("waves.shm");
      $shm_probe (tb, "ACSM");
   end

endmodule
