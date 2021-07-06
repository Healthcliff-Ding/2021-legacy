`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 12:03:49
// Design Name: 
// Module Name: dataMemory_tb
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


module dataMemory_tb(    );
    reg Clk;
    reg [31:0] Addr;
    reg [31:0] writeData;
    reg memWrite;
    reg memRead;
    wire [31:0] readData;
    
    Memory u1 (
        .Clk(Clk),
        .addr(Addr),
        .wrtData(writeData),
        .memWrt(memWrite),
        .memRd(memRead),
        .rdData(readData)
    );
    
    parameter PERIOD = 50;
    always #(PERIOD*2) Clk = ~Clk;
    
    initial begin
        Clk = 0;
        memWrite = 0;
        memRead = 0;
        Addr = 0;
        writeData = 0;
        
        #185;
        memWrite = 1;
        Addr = 7;
        writeData = 128;
        
        #100;
        memWrite = 1;
        writeData = 32'hffffffff;
        Addr = 6;
        
        #185;
        memRead = 1;
        memWrite = 0;
        Addr = 7;
        
        #100;
        memWrite = 1;
        Addr = 8; // 由于是时钟下降沿才往里面写，因此此时读信号读的是原始数据0
        writeData = 32'haaaaaaaa;
        
        #100;
        memWrite = 0;
        memRead = 1;
        Addr = 6;
        
        #80;
        memWrite = 0;
        memRead = 1;
        Addr = 8;
    end
    
endmodule
