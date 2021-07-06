`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 10:50:02
// Design Name: 
// Module Name: Alu_tb
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


module Alu_tb(    );

reg [3:0] AluCtr;
reg [31:0] Op1;
reg [31:0] Op2;
wire Zero;
wire [31:0] AluRes;

Alu u2 (
    .aluCtr(AluCtr),
    .op1(Op1),
    .op2(Op2),
    .zero(Zero),
    .aluRes(AluRes)
);

initial begin
    AluCtr = 0;
    Op1 = 0;
    Op2 = 0;
    #100;
    Op1 = 15;
    Op2 = 10;
    #100;
    AluCtr = 4'b0001;
    #100;
    AluCtr = 4'b0010;
    #100;
    AluCtr = 4'b0110;
    #100;
    Op1 = 10;
    Op2 = 15;
    #100;
    Op1 = 15;
    Op2 = 10;
    AluCtr = 4'b0111;
    #100;
    Op1 = 10;
    Op2 = 15;
    #100;
    Op1 = 1;
    Op2 = 1;
    AluCtr = 4'b1100;
    #100;
    Op1 = 16;
    
end

endmodule
