`include "defines.svh"

module fmm (
    clk,
    rst,
    data,
    vsize,
    fsize,
    en,
    merged
);

input clk;
input rst;
input [(`DW-1):0] data  [`DEEP-1:0];
input [(`CW-1):0] vsize [`DEEP-1:0];
input [(`CW-1):0] fsize [`DEEP-1:0];
input en;
output reg [(`MW-1):0] merged;

always @(posedge clk)
begin
    if (rst)
    begin
        merged <= {`MW{1'b0}};
    end
    else if (en)
    begin
        begin
        end
    end
end

endmodule

