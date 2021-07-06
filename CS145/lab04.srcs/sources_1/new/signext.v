`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/25 21:00:02
// Design Name: 
// Module Name: signext
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module signext(
    input [15:0] intIn,
    output reg [31:0] intOut
    );
    initial begin
        intOut = 0;
    end
    always @ (intIn)
    begin
        intOut[31:16] = 16'h0000;
        intOut[15:0] = intIn;
        if (intIn[15])
        begin
            intOut[31:16] = 16'hffff;
        end
    end
    
endmodule
