`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/25 21:06:46
// Design Name: 
// Module Name: singext_tb
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


module singext_tb();
    reg [15:0] intIn;
    wire [31:0] intOut;
    
    signext u2 (
        .intIn(intIn),
        .intOut(intOut)
    );
    
    initial begin
        intIn = 0;
        
        #100 intIn = 1;
        #100 intIn = -1;
        #100 intIn = 2;
        #100 intIn = -2;
        #100 intIn = 100;
        #100 intIn = -100;
        
    end
endmodule
