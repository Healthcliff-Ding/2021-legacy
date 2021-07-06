`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/25 21:34:38
// Design Name: 
// Module Name: Top
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


module Top(    
    input CLK,
    input Reset
);
    // 我是不是应该把线的名字都给出来
    // 指令内存段
    reg [31:0] CodeSegment[0:127];
    // PC
    wire [31:0] JumpAddress;
    wire [31:0] NextAddress;
    wire [31:0] BranchAddress;
    wire BranchCtrl;
    wire JumpAndLink;
    wire [31:0] BranchTakeAddress;
    // Instruction Memory
    wire [31:0] Instruction; // InstMem的输出
    wire RegDst,
        Jump,
        Branch,
        MemRead,
        MemToReg,
        MemWrite,
        SignOrZero;
    wire [1:0] ALUOp;
    wire ALUSrc1, ALUSrc2, RegWrite;
    // Register File
    wire [4:0] WriteReg;
    wire [31:0] WriteData;
    wire [31:0] ReadData1;
    wire [31:0] ReadData2;
    // Memory File
    wire [31:0] MemReadData;
    // ALU Control
    wire [5:0] Function;
    wire [3:0] ALUCtrlSignal;
    // Sign Extension
    wire [31:0] ExtOut;
    wire [31:0] ZeroExtOut;
    wire [31:0] SignExtOut;
    wire [31:0] Shamt;
    // ALU
    wire Zero;
    wire [31:0] ALUOperand1;
    wire [31:0] ALUOperand2;
    wire [31:0] ALUResult;
    wire JumpReg;
    // 用来算地址的ALU就不走线了
    // 多路复用器的输出走根线
    
    reg [31:0] ReadAddress_r;
    
    Ctr Control(
        .opCode(Instruction[31:26]),
        .func(Instruction[5:0]),
        .regDst(RegDst),
        .aluSrc1(ALUSrc1),
        .aluSrc2(ALUSrc2),
        .memToReg(MemToReg),
        .memWrite(MemWrite),
        .branch(Branch),
        .memRead(MemRead),
        .aluOp(ALUOp),
        .jump(Jump),
        .jal(JumpAndLink),
        .signorzero(SignOrZero),
        .regWrite(RegWrite)
    );
    
    Alu ALU(
        .aluCtr(ALUCtrlSignal),
        .op1(ALUOperand1),
        .op2(ALUOperand2),
        .aluRes(ALUResult),
        .zero(Zero),
        .jr(JumpReg)
    );
    
    AluCtr ALUControl(
        .func(Function),
        .aluOp(ALUOp),
        .aluCtr(ALUCtrlSignal)
    );
    
    Register RegFile(
        .rdReg1(Instruction[25:21]),
        .rdReg2(Instruction[20:16]),
        .clk(CLK),
        .wrtReg(WriteReg),
        .wrtData(WriteData),
        .regWrt(RegWrite),
        .reset(Reset),
        .rdData1(ReadData1),
        .rdData2(ReadData2)
    );
    
    Memory MemFile(
        .Clk(CLK),
        .addr(ALUResult),
        .wrtData(ReadData2),
        .memWrt(MemWrite),
        .memRd(MemRead),
        .rdData(MemReadData)
    );
    
    signext SignExtend(
        .intIn(Instruction[15:0]),
        .intOut(SignExtOut)
    );
    
    // always 模块block内是串行执行的
    // 但是模块之间是并行执行的
    // 在激励文件里头把汇编放到内存里头
    assign Instruction = CodeSegment[ReadAddress_r >> 2];
    assign JumpAddress = JumpReg ? ALUResult : {NextAddress[31:28], Instruction[25:0]<<2};
    assign BranchAddress = (SignExtOut<<2) + NextAddress;
    assign NextAddress = ReadAddress_r + 4;
    assign Shamt[5:0] = Instruction[10:6];
    assign Shamt[31:6] = 0;
    assign ZeroExtOut = {16'h0000, Instruction[15:0]};
    assign WriteReg = JumpAndLink ? 5'b11111 : ( RegDst ? Instruction[15:11] : Instruction[20:16] );
    assign BranchCtrl = Branch & Zero;
    assign BranchTakeAddress = BranchCtrl ? BranchAddress : NextAddress;
    assign Function = (ALUOp == 2'b11) ? Instruction[31:26] : Instruction[5:0];
    assign ExtOut = SignOrZero ? SignExtOut : ZeroExtOut;
    assign ALUOperand1 = ALUSrc1 ? Shamt : ReadData1;
    assign ALUOperand2 = ALUSrc2 ? ExtOut : ReadData2;
    assign WriteData = JumpAndLink ? NextAddress : ( MemToReg ? MemReadData : ALUResult );
    
    initial begin
        $readmemb("./test/code5.txt", CodeSegment);
    end
    
    always @ (posedge CLK)
    begin
        if (Reset) ReadAddress_r = 32'h00000000;
        else begin
        ReadAddress_r = Jump ? JumpAddress : BranchTakeAddress;
        end
    end
    
endmodule
