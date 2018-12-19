`include "defines.svh"

module dgen (
    clk,
    rst,
    data,
    vsize,
    fsize,
    en
);

parameter id = 0;

input clk;
input rst;
output reg [(`DW-1):0] data;
output reg [(`CW-1):0] vsize;
output reg [(`CW-1):0] fsize;
input en;

always @(posedge clk) begin
    if (rst) begin
        data <= {`DW{1'b0}};
        vsize <= {`CW{1'b0}};
        fsize <= {`CW{1'b0}};
    end
    else if (en) begin
        data <= id;
        vsize <= 4;
        fsize <= 4;
    end
    else begin
        data <= {`DW{1'b0}};
        vsize <= {`CW{1'b0}};
        fsize <= {`CW{1'b0}};
    end
end

endmodule
