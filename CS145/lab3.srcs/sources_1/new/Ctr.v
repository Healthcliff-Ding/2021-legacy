`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 08:19:54
// Design Name: 
// Module Name: Ctr
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


module Ctr(
    input [5:0] opCode,
    output regDst,
    output aluSrc,
    output memToReg,
    output regWrite,
    output memRead,
    output memWrite,
    output branch,
    output [1:0] aluOp,
    output jump
    );
    wire [5:0] OpCode;
    reg RegDst;
    reg AluSrc;
    reg MemToReg;
    reg RegWrite;
    reg MemRead;
    reg MemWrite;
    reg Branch;
    reg [1:0] AluOp;
    reg Jump;
    
    always @ (OpCode)
    begin
        case(OpCode)
        6'b000000: //R-format
        begin
            RegDst = 1;
            AluSrc = 0;
            MemToReg = 0;
            RegWrite = 1;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b10;
            Jump = 0;
        end
        6'b100011:
        begin
            RegDst = 0;
            AluSrc = 1;
            MemToReg = 1;
            RegWrite = 1;
            MemRead = 1;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 0;
        end
        6'b101011:
        begin
            RegDst = 0;
            AluSrc = 1;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 1;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 0;
        end
        6'b000100:
        begin
            RegDst = 0;
            AluSrc = 0;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 0;
            Branch = 1;
            AluOp = 2'b01;
            Jump = 0;
        end
        6'b000010:
        begin
            RegDst = 0;
            AluSrc = 0;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 1;
        end
        default:
        begin
            RegDst = 0;
            AluSrc = 0;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 0;
        end
        endcase
   end
   
assign OpCode = opCode;
assign regDst = RegDst;
assign aluSrc = AluSrc;
assign regWrite = RegWrite;
assign memRead = MemRead;
assign memToReg = MemToReg;
assign memWrite = MemWrite;
assign branch = Branch;
assign aluOp = AluOp;
assign jump = Jump;
 
endmodule
