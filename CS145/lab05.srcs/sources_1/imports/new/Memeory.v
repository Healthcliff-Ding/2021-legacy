`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 11:58:13
// Design Name: 
// Module Name: Memeory
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


module Memory(
    input Clk,
    input [31:0] addr,
    input [31:0] wrtData,
    input memWrt, 
    input memRd, 
    output reg [31:0] rdData
    );
    
    reg [31:0] Mem [127:0];
    integer i;
    initial begin
        for (i=0; i<127; i = i+1) Mem[i] = 0;
        $readmemh("./test/data3.txt", Mem);
    end
    
    
    always @ (*)
    begin
        rdData = Mem[addr];
    end
    
    always @ (negedge Clk)
    begin
        if (memWrt && addr)
            Mem[addr] <= wrtData;
    end

endmodule
