`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 11:06:59
// Design Name: 
// Module Name: Register
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


module Register(
    input [25:21] rdReg1,
    input [20:16] rdReg2,
    input [4:0] wrtReg,
    input [31:0] wrtData,
    input regWrt,
    input clk,
    input reset,
    output [31:0] rdData1,
    output [31:0] rdData2
    );
    
    reg [31:0] regFile[0:31];
    integer i;
    initial begin
    for (i = 0; i < 32;i = i + 1) regFile[i] <= 0;
    end
    
    assign rdData1 = (rdReg1 != 0) ? regFile[rdReg1] : 0;
    assign rdData2 = (rdReg2 != 0) ? regFile[rdReg2] : 0;
    
    always @ (negedge clk)
    begin
        if (reset) begin
            for (i = 0; i < 32; i = i + 1) regFile[i] <= 0;
        end
        else if (regWrt) begin
            regFile[wrtReg] <= wrtData;
            regFile[0] <= 0;
        end
    end
    
endmodule
