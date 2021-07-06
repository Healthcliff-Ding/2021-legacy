`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 10:12:21
// Design Name: 
// Module Name: AluCtr2_tb
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


module AluCtr2_tb(    );
    reg [5:0] Func;
    reg [1:0] AluOp;
    wire [3:0] AluCtr;
    
    AluCtr u1 (
        .func(Func),
        .aluOp(AluOp),
        .aluCtr(AluCtr)
    );
    
initial begin
    AluOp = 0;
    Func = 0;
    #160;
    AluOp = 2'b01;
    #60; 
    AluOp = 2'b10;
    #60;
    Func = 6'b000010;
    #60;
    Func = 6'b000100;
    #60;
    Func = 6'b000101;
    #60;
    Func = 6'b001010;
end

endmodule
