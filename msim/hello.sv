`timescale 1ns/1ns

module hello;

reg clk=0;
always #5 clk = ~clk;

import "DPI-C" function void sw_call();

always @ (posedge clk) begin
	sw_call();
end

initial begin
	$display ("Verilog simulation in windows environment");
	#1000 $finish;
end

endmodule
