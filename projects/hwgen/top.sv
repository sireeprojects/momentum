`include "defines.svh"

module top;

reg clk=0;
reg [(`DW-1):0] data  [`DEEP-1:0];
reg [(`CW-1):0] vsize [`DEEP-1:0];
reg [(`CW-1):0] fsize [`DEEP-1:0];
reg [(`MW-1):0] merged;
reg en = 1;

// reset
greset gen_reset (clk, rst);

// clock
always #3.2 clk = ~clk;

genvar id;
generate
    for (id=0; id<`DEEP; id++)
    begin
        dgen #(id)
        inst (clk, rst, data[id], vsize[id], fsize[id], en);
    end
endgenerate

fmm fmm (clk, rst, data, vsize, fsize, en, merged);

endmodule
