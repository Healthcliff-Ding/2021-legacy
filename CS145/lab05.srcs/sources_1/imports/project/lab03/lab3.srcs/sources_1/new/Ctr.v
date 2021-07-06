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
    input [5:0] func,
    output regDst,
    output aluSrc1,
    output aluSrc2,
    output memToReg,
    output regWrite,
    output memRead,
    output memWrite,
    output branch,
    output [1:0] aluOp,
    output jump,
    output jal,
    output signorzero
    );
    wire [5:0] Func;
    wire [5:0] OpCode;
    reg RegDst;
    reg AluSrc1;
    reg AluSrc2;
    reg MemToReg;
    reg RegWrite;
    reg MemRead;
    reg MemWrite;
    reg Branch;
    reg [1:0] AluOp;
    reg Jump;
    reg JAL;
    reg SignOrZero;
    
    always @ (OpCode or Func)
    begin
        JAL = 0;
        casex(OpCode)
        6'b000000: // R-format
        begin
            RegDst = 1;
            AluSrc2 = 0;
            MemToReg = 0;
            RegWrite = 1;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b10;
            Jump = (Func === 6'b001000) ? 1: 0;
            AluSrc1 = (Func[5:2] === 4'b0000) ? 1 : 0;
            SignOrZero = 0;
        end
        6'b100011: // lw
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 1;
            MemToReg = 1;
            RegWrite = 1;
            MemRead = 1;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 0;
            SignOrZero = 1;
        end
        6'b101011: // sw
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 1;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 1;
            Branch = 0;
            AluOp = 2'b11;
            Jump = 0;
            SignOrZero = 1;
        end
        6'b00010x: // beq, bne
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 0;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 0;
            Branch = 1;
            AluOp = 2'b01;
            Jump = 0;
            SignOrZero = 1;
        end
        
        6'b000010: // jump
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 0;
            MemToReg = 0;
            RegWrite = 0;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 1;
            SignOrZero = 0;
        end
        6'b000011: // jal
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 0;
            MemToReg = 0;
            RegWrite = 1;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b00;
            Jump = 1;
            SignOrZero = 0;
            JAL = 1;
        end
        6'b001xxx: // I-format
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 1;
            MemToReg = 0;
            RegWrite = 1;
            MemRead = 0;
            MemWrite = 0;
            Branch = 0;
            AluOp = 2'b11;
            Jump = 0;
            SignOrZero = (OpCode[2:0] == 3'b0x0) ? 1 : 0;
        end
        default:
        begin
            RegDst = 0;
            AluSrc1 = 0;
            AluSrc2 = 0;
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
   
assign Func = func;
assign OpCode = opCode;
assign regDst = RegDst;
assign aluSrc1 = AluSrc1;
assign aluSrc2 = AluSrc2;
assign regWrite = RegWrite;
assign memRead = MemRead;
assign memToReg = MemToReg;
assign memWrite = MemWrite;
assign branch = Branch;
assign aluOp = AluOp;
assign jump = Jump;
assign signorzero = SignOrZero;
assign jal = JAL;
 
endmodule
