`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 10:40:08
// Design Name: 
// Module Name: Alu
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


module Alu(
    input [3:0] aluCtr,
    input [31:0] op1,
    input [31:0] op2,
    output [31:0] aluRes,
    output zero,
    output overflow,
    output jr
    );
    wire [3:0] AluCtr;
    wire [31:0] Op1;
    wire [31:0] Op2;
    reg JR;
    reg OF;
    reg [31:0] AluRes;
    reg [31:0] tmp1;
    reg [31:0] tmp2;
    reg [1:0] signFlag;
    
    always @ (AluCtr or op1 or op2)
    begin
        JR = 0;
        case(AluCtr)
        4'b0000: // and, andi
        begin
            AluRes = op1 & op2;
        end
        4'b0001: // or, ori
        begin
            AluRes = op1 | op2;
        end
        4'b0010: // add, addu
        begin
            { OF, AluRes } = op1 + op2;
        end
        4'b0110: // sub, subu
        begin
            { OF, AluRes } = op1 - op2;
        end
        4'b1001: // ~sub
        begin
            AluRes = ~(op1 - op2);
        end
        4'b0111: // slt
        begin
            if (op1 < op2)
                AluRes = 1;
            else
                AluRes = 0;
        end
        4'b1000: // sltu
        begin
            tmp1 = { 1'b0, op1[30:0] };
            tmp2 = { 1'b0, op2[30:0] };
            signFlag = { op1[31], op2[31] };
            if (signFlag == 2'b00)
                AluRes = (tmp1 < tmp2) ? 1: 0;
            else if (signFlag == 2'b01)
                AluRes = 1;
            else if (signFlag == 2'b10)
                AluRes = 0;
            else
                AluRes = (tmp1 < tmp2) ? 1: 0;
        end
        4'b1100: // nor
        begin
            AluRes = ~(op1 | op2);
        end
        4'b1101: // xor
        begin
            AluRes = op1 ^ op2;
        end
        4'b0101: // srl
        begin
            AluRes = op2 >> op1;
            AluRes = AluRes & ((1<<(32-op1))-1);
        end
        4'b1010: // sra
        begin
            AluRes = op2 >>> op1;
        end
        4'b0011: // ret op1
        begin
            AluRes = op1;
        end
        4'b0100: // sll
        begin
            AluRes = op2 << op1;
        end
        4'b1110: // lui
        begin
            AluRes[31:16] = op2[15:0];
            AluRes[15: 0] = 0;
        end
        4'b1111: // jr
        begin
            AluRes = op1;
            JR = 1;
        end
        endcase
    end
    assign AluCtr = aluCtr;
    assign aluRes = AluRes;
    assign zero = (AluRes == 0) ? 1 : 0;
    assign jr = JR;
    assign overflow = OF;
endmodule
