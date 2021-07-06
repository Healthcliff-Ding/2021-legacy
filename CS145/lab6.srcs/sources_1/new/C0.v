`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2021/05/29 22:43:30
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
    input RESET);
    
    reg Stall;
    reg Flush;
    /********** IF ***********/
    // IF interface
    wire [31:0] JumpAddr;
    wire [31:0] BranchAddr;
    wire PCSrc;
    wire Jump; // from EX, serve as selection signal for IF and stall signal for ID
    reg [31:0] IF_ID_NextAddr;
    reg [31:0] IF_ID_Inst;

    // IF internal circuit
    // ROM
    reg [31:0] CodeSegment[0:127];
    reg [31:0] PC;
    
    wire [31:0] IF_NextAddr;
    wire [31:0] IF_Inst;
    assign IF_NextAddr = PC + 4;
    assign IF_Inst = CodeSegment[PC >> 2];

    /*********** ID ***********/
    // ID interface
    wire WB_RegWrite;
    wire [4:0] WB_WriteReg;
    wire [31:0] WB_WriteData;
    reg [8:0] ID_EX_EXCtrl; // { RegDst, ALUSrc1, ALUSrc2, SignOrZero, [1:0]ALUOp, Jump, JAL, Branch}
    reg [1:0] ID_EX_MACtrl; // { MemWrite, MemRead }
    reg [1:0] ID_EX_WBCtrl; // { RegWrite, MemToReg }
    reg [31:0] ID_EX_ReadData1;
    reg [31:0] ID_EX_ReadData2;
    reg [31:0] ID_EX_NextAddr;
    reg [31:0] ID_EX_JumpAddr;
    reg [31:0] ID_EX_SignExt;
    reg [31:0] ID_EX_ZeroExt;
    reg [31:0] ID_EX_Shamt;
    reg [5:0] ID_EX_Func;
    reg [4:0] ID_EX_RD;
    reg [4:0] ID_EX_RT;
    // ID internal circuit
    // 首字母小写的全部为临时变量
    wire [31:0] ID_Inst;
    wire regDst;
    wire aluSrc1;
    wire aluSrc2;
    wire memToReg;
    wire memWrite;
    wire branch;
    wire memRead;
    wire [1:0] aluOp;
    wire jump;
    wire jal;
    wire signOrZero;
    wire regWrite;

    assign ID_Inst = IF_ID_Inst;

    Ctr Control(
        .opCode(ID_Inst[31:26]),
        .func(ID_Inst[5:0]),
        .regDst(regDst),
        .aluSrc1(aluSrc1),
        .aluSrc2(aluSrc2),
        .memToReg(memToReg),
        .memWrite(memWrite),
        .branch(branch),
        .memRead(memRead),
        .aluOp(aluOp),
        .jump(jump),
        .jal(jal),
        .signorzero(signOrZero),
        .regWrite(regWrite)
    );

    wire [31:0] readData1;
    wire [31:0] readData2;

    Register RegFile(
        .rdReg1(ID_Inst[25:21]),
        .rdReg2(ID_Inst[20:16]),
        .clk(CLK),
        .reset(RESET),
        .wrtReg(WB_WriteReg),
        .wrtData(WB_WriteData),
        .regWrt(WB_RegWrite),
        .rdData1(readData1),
        .rdData2(readData2)
    );

    wire [31:0] signExt;

    signext SignExtend(
        .intIn(ID_Inst[15:0]),
        .intOut(signExt)
    );
    
    wire [31:0] bypassData1;
    wire [31:0] bypassData2;

    /*********** EX ***********/
    // EX interface
    reg [1:0] EX_MA_MACtrl; // { MemWrite, MemRead }
    reg [1:0] EX_MA_WBCtrl; // { RegWrite, MemToReg }
    reg [31:0] EX_MA_ALUResult;
    reg [31:0] EX_MA_WriteData;
    reg [31:0] EX_MA_NextAddr;
    reg [4:0] EX_MA_WriteReg;

    // EX internal circuit
    wire zero;
    wire jr;
    wire [3:0] aluCtrlSignal;
    wire [31:0] aluOprd1;
    wire [31:0] aluOprd2;
    wire [31:0] EX_BranchAddr;
    wire [31:0] aluResult;
    
    assign aluOprd1 = (ID_EX_EXCtrl[7]) ? ID_EX_Shamt : ID_EX_ReadData1;
    assign aluOprd2 = (ID_EX_EXCtrl[6]) ? (ID_EX_EXCtrl[5] ? ID_EX_SignExt : ID_EX_ZeroExt) : ID_EX_ReadData2;

    assign EX_BranchAddr = (ID_EX_SignExt << 2) + ID_EX_NextAddr; // 因为总是预测不跳，所以这里只需要提供跳的地址
    assign Jump = ID_EX_EXCtrl[2];
    assign PCSrc = (zero & ID_EX_EXCtrl[0]) | ID_EX_EXCtrl[2];
    assign JumpAddr = jr ? aluResult : ID_EX_JumpAddr;
    assign BranchAddr = EX_BranchAddr;
    
    Alu ALU(
        .aluCtr(aluCtrlSignal),
        .op1(aluOprd1),
        .op2(aluOprd2),
        .aluRes(aluResult),
        .zero(zero),
        .jr(jr)
    );

    AluCtr ALUControl(
        .func(ID_EX_Func),
        .aluOp(ID_EX_EXCtrl[4:3]),
        .aluCtr(aluCtrlSignal)
    );

    /*********** MA ***********/
    // MA interface
    reg [1:0] MA_WB_WBCtrl;
    reg [4:0] MA_WB_WriteReg;
    reg [31:0] MA_WB_MemReadData;
    reg [31:0] MA_WB_ALUResult;
    reg [31:0] MA_WB_NextAddr;

    // MA internal circuit
    wire [31:0] memReadData;

    Memory MemFile(
        .Clk(CLK),
        .addr(EX_MA_ALUResult),
        .wrtData(EX_MA_WriteData),
        .memWrt(EX_MA_MACtrl[1]),
        .memRd(EX_MA_MACtrl[0]),
        .rdData(memReadData)
    );

    /*********** WB ***********/
    assign WB_WriteData = (MA_WB_WriteReg == 5'b11111) ? MA_WB_NextAddr : (
        MA_WB_WBCtrl[0] ? MA_WB_MemReadData : MA_WB_ALUResult
    );
    assign WB_RegWrite = MA_WB_WBCtrl[1];
    assign WB_WriteReg = MA_WB_WriteReg;

    initial begin
        $readmemb("./test/code1.txt", CodeSegment);
        Stall = 0;
        Flush = 0;
    end
    
    /******** Forwarding ********/
    assign bypassData1 = ( // EX_ID data hazard1
                !((( ID_EX_EXCtrl[1] ? 5'b11111 : (ID_EX_EXCtrl[8] ? ID_EX_RD : ID_EX_RT) ) === ID_Inst[25:21])
                  && ID_EX_WBCtrl[1]) // ID_EX pipeline register is stalled with data hazard
            ) ? readData1 : (
                ID_EX_WBCtrl[0] ? memReadData : aluResult // source is memory or ALU
            );
    assign bypassData2 = ( // EX_ID data hazard
                !((( ID_EX_EXCtrl[1] ? 5'b11111 : (ID_EX_EXCtrl[8] ? ID_EX_RD : ID_EX_RT) ) === ID_Inst[20:16])
                  && ID_EX_WBCtrl[1]) // ID_EX pipeline register is stalled with data hazard
            ) ? readData2 : (
                ID_EX_WBCtrl[0] ? memReadData : aluResult // source is memory or ALU
            );


    always @(posedge CLK) 
    begin
        if (!Stall) begin
            // set PC
            if (RESET) begin
                PC = 32'h00000000; // 别的也要set
            end
            else if (PCSrc) begin
                PC = Jump ? JumpAddr : BranchAddr;
                Flush = 1;
            end
            else begin
                PC = IF_NextAddr;
            end

            // set IF_ID pipeline register
            IF_ID_NextAddr <= IF_NextAddr;
            IF_ID_Inst <= IF_Inst;
        end
        
        if (!Stall) begin
            // set ID_EX pipeline register
            ID_EX_EXCtrl <= { regDst, aluSrc1, aluSrc2, signOrZero, aluOp, jump, jal, branch };
            ID_EX_MACtrl <= { memWrite, memRead };
            ID_EX_WBCtrl <= { regWrite, memToReg };
            ID_EX_ReadData1 <= ( // MA_ID data hazard
                !(EX_MA_WriteReg === ID_Inst[25:21]
                  && EX_MA_WBCtrl[1])
                ) ? bypassData1 : (
                EX_MA_WBCtrl[0] ? memReadData : EX_MA_ALUResult
            );
            ID_EX_ReadData2 <= ( // MA_ID data hazard
                !(EX_MA_WriteReg === ID_Inst[20:16]
                  && EX_MA_WBCtrl[1])
                ) ? bypassData2 : (
                EX_MA_WBCtrl[0] ? memReadData : EX_MA_ALUResult
            );
            ID_EX_NextAddr <= IF_ID_NextAddr;
            ID_EX_JumpAddr <= { IF_ID_NextAddr[31:28], ID_Inst[25:0] << 2};
            ID_EX_SignExt <= signExt;
            ID_EX_ZeroExt <= { 16'h0000, ID_Inst[15:0] };
            ID_EX_Shamt <= { {27{1'b0}}, ID_Inst[10:6] };
            ID_EX_Func <= (aluOp == 2'b11) ? ID_Inst[31:26] : ID_Inst[5:0];
            ID_EX_RD <= ID_Inst[15:11];
            ID_EX_RT <= ID_Inst[20:16];
        end
        
        // set EX_MA pipeline register
        EX_MA_MACtrl <= ID_EX_MACtrl;
        EX_MA_WBCtrl <= ID_EX_WBCtrl;
        EX_MA_ALUResult <= aluResult;
        EX_MA_WriteData <= ID_EX_ReadData2;
        EX_MA_NextAddr <= ID_EX_NextAddr;
        EX_MA_WriteReg <= ID_EX_EXCtrl[1] ? 5'b11111 : (ID_EX_EXCtrl[8] ? ID_EX_RD : ID_EX_RT);
        
        // set MA_WB pipeline register
        MA_WB_ALUResult <= EX_MA_ALUResult;
        MA_WB_MemReadData <= memReadData;
        MA_WB_NextAddr <= EX_MA_NextAddr;
        MA_WB_WriteReg <= EX_MA_WriteReg;
        MA_WB_WBCtrl <= EX_MA_WBCtrl;
    end

    always @(negedge CLK) begin

        if (Stall) begin
            Stall <= 0;
        end
        else if (
            ((( ID_EX_EXCtrl[1] ? 5'b11111 : (ID_EX_EXCtrl[8] ? ID_EX_RD : ID_EX_RT) ) === ID_Inst[25:21]) ||
             (( ID_EX_EXCtrl[1] ? 5'b11111 : (ID_EX_EXCtrl[8] ? ID_EX_RD : ID_EX_RT) ) === ID_Inst[20:16]))
            && ID_EX_WBCtrl[1] && ID_EX_WBCtrl[0]
        ) begin
            Stall <= 1;
        end
        else begin
            Stall <= 0;
        end

        if (Flush) begin
            IF_ID_NextAddr <= 0;
            IF_ID_Inst <= 0;

            ID_EX_EXCtrl <= 0;
            ID_EX_MACtrl <= 0;
            ID_EX_WBCtrl <= 0;
            ID_EX_ReadData1 <= 0;
            ID_EX_NextAddr <= 0;
            ID_EX_JumpAddr <= 0;
            ID_EX_SignExt <= 0;
            ID_EX_ZeroExt <= 0;
            ID_EX_Shamt <= 0;
            ID_EX_Func <= 0;
            ID_EX_RD <= 0;
            ID_EX_RT <= 0;

            Flush <= 0;
        end

    end

endmodule
