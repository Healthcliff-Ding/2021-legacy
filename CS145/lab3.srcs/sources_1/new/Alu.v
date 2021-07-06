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
    output zero
    );
    wire [3:0] AluCtr;
    wire [31:0] Op1;
    wire [31:0] Op2;
    reg Zero;
    reg [31:0] AluRes;
    
    always @ (op1 or op2 or AluCtr)
    begin
        case(AluCtr)
        4'b0000:
        begin
            AluRes = op1 & op2;
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        4'b0001:
        begin
            AluRes = op1 | op2;
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        4'b0010:
        begin
            AluRes = op1 + op2;
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        4'b0110:
        begin
            AluRes = op1 - op2;
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        4'b0111:
        begin
            if (op1 < op2)
                AluRes = 1;
            else
                AluRes = 0;
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        4'b1100:
        begin
            AluRes = ~(op1 | op2);
            if (AluRes == 0)
                Zero = 1;
            else
                Zero = 0;
        end
        endcase
    end
    assign AluCtr = aluCtr;
    assign aluRes = AluRes;
    assign zero = Zero;
endmodule
