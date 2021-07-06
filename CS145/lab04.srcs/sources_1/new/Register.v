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
    output reg [31:0] rdData1,
    output reg [31:0] rdData2
    );
    
    reg [31:0] regFile[0:31];
    integer i;
    initial begin
    for (i = 0; i < 32;i = i + 1) regFile[i] = 0;
    end
    
    always @(rdReg1 or rdReg2 or wrtReg)
    begin
        rdData1 = regFile[rdReg1];
        rdData2 = regFile[rdReg2];
    end
    
    always @ (negedge clk && regWrt)
    begin
        if (regWrt)
            regFile[wrtReg] = wrtData;
    end
endmodule
