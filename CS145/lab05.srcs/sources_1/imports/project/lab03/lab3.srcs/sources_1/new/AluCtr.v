`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/19 09:52:58
// Design Name: 
// Module Name: AluCtr
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


module AluCtr(
    input [5:0] func, 
    input [1:0] aluOp,
    output [3:0] aluCtr
    );
    wire [1:0] AluOp;
    wire [5:0] Func;
    reg [3:0] AluCtr;
    
    always @ (AluOp or Func)
    begin
        casex(AluOp)
        2'b00: // jump, lw, jal
        begin 
            AluCtr = 4'b0010; // add
        end
        2'b01: // beq, bne
        begin
            if (~Func[0]) // func is actually OpCode
                AluCtr = 4'b0110; // sub
            else
                AluCtr = 4'b1001; // ~sub
        end
        2'b10: // R-type
        begin
            casex(Func)
            6'b10000x: // add, addu
            begin
                AluCtr = 4'b0010;
            end
            6'b10001x: // sub, subu
            begin
                AluCtr = 4'b0110;
            end
            6'b100100: // and
            begin
                AluCtr = 4'b0000;
            end
            6'b100101: // or
            begin
                AluCtr = 4'b0001;
            end
            6'b100111: // nor
            begin
                AluCtr = 4'b1100;
            end
            6'b101010: // slt
            begin
                AluCtr = 4'b0111;
            end
            6'b101011: // sltu
            begin
                AluCtr = 4'b1000;
            end
            6'b000x00: // sll, sllv
            begin
                AluCtr = 4'b0100;
            end
            6'b000x10: // srl, srlv
            begin
                AluCtr = 4'b0101;
            end
            6'b000x11: // sra, srav
            begin
                AluCtr = 4'b1010;
            end
            6'b001000: // jr
            begin
                AluCtr = 4'b1111;
            end
            default:
            begin
                AluCtr = 4'b0110;
            end
            endcase
        end
        2'b11: // I-type, sw
        begin
            casex(Func)// Func is multiplexed by opCode
            6'b00100x: // addi, addiu
            begin
                AluCtr = 4'b0010;
            end
            6'b001100: // andi
            begin
                AluCtr = 4'b0000;
            end
            6'b001101: // ori
            begin
                AluCtr = 4'b0001;
            end
            6'b001110: // xori
            begin
                AluCtr = 4'b1101;
            end
            6'b001010: // slti
            begin
                AluCtr = 4'b0111;
            end
            6'b001011: // sltiu
            begin
                AluCtr = 4'b1000;
            end
            6'b001111: // lui
            begin
                AluCtr = 4'b1110;
            end
            6'b101011: //sw
            begin
                AluCtr = 4'b0010; // add
            end
            endcase 
        end
        endcase
    end
    
    assign AluOp = aluOp;
    assign Func = func;
    assign aluCtr = AluCtr;
endmodule



