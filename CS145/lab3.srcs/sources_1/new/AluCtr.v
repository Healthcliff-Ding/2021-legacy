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
    
    always @(AluOp or Func)
    begin
        casex(AluOp)
        2'b00:
        begin 
            AluCtr = 4'b0010;
        end
        2'b01:
        begin
            AluCtr = 4'b0110;
        end
        2'b1x:
        begin
            casex(Func)
            6'bxx0000:
            begin
                AluCtr = 4'b0010;
            end
            6'bxx0010:
            begin
                AluCtr = 4'b0110;
            end
            6'bxx0100:
            begin
                AluCtr = 4'b0000;
            end
            6'bxx0101:
            begin
                AluCtr = 4'b0001;
            end
            6'bxx1010:
            begin
                AluCtr = 4'b0111;
            end
            default:
            begin
                AluCtr = 4'b0110;
            end
            endcase
        end
        endcase
    end
    
    assign AluOp = aluOp;
    assign Func = func;
    assign aluCtr = AluCtr;
endmodule



