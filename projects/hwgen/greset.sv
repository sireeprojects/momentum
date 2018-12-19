module greset (
    clk,
    reset
);

// Inputs
input clk;

// Outputs
output reg reset;

// Module parameters
parameter interval=100;

// local net
integer counter=0;

initial 
reset = 1;

always @(posedge clk) begin
    counter = counter+1;
    if (counter>=interval) begin
        reset <= 0;
    end
end

endmodule
