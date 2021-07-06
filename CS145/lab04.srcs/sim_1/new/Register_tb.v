`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 11:24:28
// Design Name: 
// Module Name: Register_tb
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


module Register_tb(    );
    reg [25:21] readReg1;
    reg [20:16] readReg2;
    reg [4:0] writeReg;
    reg [31:0] writeData;
    reg regWrite;
    reg clk;
    wire [31:0] readData1;
    wire [31:0] readData2;
    
    Register u0 (
        .rdReg1(readReg1),
        .rdReg2(readReg2),
        .wrtReg(writeReg),
        .wrtData(writeData),
        .regWrt(regWrite),
        .rdData1(readData1),
        .rdData2(readData2),
        .clk(clk)
    );
    
    parameter PERIOD = 50;
    always #(PERIOD*2) clk = !clk;
    
    initial begin
        clk = 0;
        readReg1 = 0;
        readReg2 = 0;
        writeReg = 0;
        writeData = 0;
        regWrite = 0;
        
        #185;
        regWrite = 1;
        writeReg = 5'b10101;
        writeData = 32'b11111111111111110000000000000000;
        
        #200;
        writeReg = 5'b01010;
        writeData = 32'b00000000000000001111111111111111;
        
        #200;
        regWrite = 0;
        writeReg = 5'b00000;
        writeData = 32'b11111111111111111111111111111111;
        
        #50;
        readReg1 = 5'b10101;
        readReg2 = 5'b01010;
    end 
endmodule
