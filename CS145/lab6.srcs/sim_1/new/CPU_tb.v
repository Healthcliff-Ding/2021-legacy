`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/31 16:26:40
// Design Name: 
// Module Name: CPU_tb
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


module CPU_tb(    );
    reg CLK, Reset;
    
    Top CPU(
        .CLK(CLK),
        .RESET(Reset)
    );

    parameter CYCLE = 10;
    always #(CYCLE) CLK = ~CLK;
    
    initial begin
        CLK = 0;
        Reset = 1;
        #15 Reset = 0;
    end
endmodule
