#include <iostream>
#include <stdio.h>

// 为了简便开的一些全局变量
int Register[32] = {0}, IR, inst(0);
unsigned int PC, RW, y(0), n(0);
char Memory[0x100000] = {0};
char tmp[10];
bool pred_branch, stall1 = false, flush2 = false;
enum INSTRUCTION{
    LUI,      //U
    AUIPC,    //U  
    JAL,      //J
    JALR,     //I
    BEQ,      //B
    BNE,      
    BLT,      
    BGE,      
    BLTU,     
    BGEU,     
    LB,       //I
    LH,       
    LW,       
    LBU,      
    LHU,      
    SB,       //S
    SH,       
    SW,       
    ADDI,     //I
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,      //R
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    OR,
    AND,
    SRL,
    SRA,
    NOP
}type[3];


// 需要实现四个锁存器!
struct pipeline_register1{
    int IR, NPC;
    void clear() { IR = NPC = 0; }
}IF_ID;
struct pipeline_register2{
    int NPC, A, B, imm, IR;
    void clear() { NPC = A = B = imm = IR = 0; }
}ID_EX;
struct pipeline_register3{
    int ALUo, B, IR;
    bool cond; // 判断分支是否成功
    void clear() { ALUo = B = IR = cond = 0; }
}EX_MEM;
struct pipeline_register4{
    int ALUo, LMD, IR;
    void clear() { ALUo = LMD = IR = 0; }
}MEM_WB;

// branch target buffer
int BTB[0x100000] = {0};


// 先写辅助函数,模拟一个器件
void visit_IR(int inst, INSTRUCTION &opt)
{
    // 先看 OPCODE
    switch (inst & ((1<<7)-1))
    {
    case 0b0000011: //load
        // FUNCT3
        switch((inst>>12) & 0b111)
        {
        case 0:
            opt = LB;
            break;
        case 1:
            opt = LH;
            break;
        case 2:
            opt = LW;
            break;
        case 0b100:
            opt = LBU;
            break;
        case 0b101:
            opt = LHU;
            break;
        }
        break;
    case 0b0100011: //store
        switch((inst>>12) & 0b111)
        {
        case 0:
            opt = SB;
            break;
        case 1:
            opt = SH;
            break;
        case 2:
            opt = SW;
            break;
        }
        break;
    case 0b0110011:
        // 先看 FUNCT7
        switch((inst>>25) & 0b1111111)
        {
        case 0:
            // 再看 FUNCT3
            switch((inst>>12) & 0b111)
            {
            case 0:
                opt = ADD;
                break;
            case 0b100:
                opt = XOR;
                break;
            case 0b110:
                opt = OR;
                break;
            case 0b111:
                opt = AND;
                break;
            case 0b001:
                opt = SLL;
                break;
            case 0b101:
                opt = SRL;
                break;
            case 0b010:
                opt = SLT;
                break;
            case 0b011:
                opt = SLTU;
                break;
            }
            break;
        case 0b0100000:
            // 再看 FUNCT3
            switch((inst)>>12 & 0b111)
            {
            case 0:
                opt = SUB;
                break;
            case 0b101:
                opt = SRA;
                break;
            }
        }
        break;
    case 0b0010011:
        // 先看 FUNCT3
        switch((inst>>12) & 0b111)
        {
        case 0:
            opt = ADDI;
            break;
        case 0b100:
            opt = XORI;
            break;
        case 0b110:
            opt = ORI;
            break;
        case 0b111:
            opt = ANDI;
            break;
        case 0b001:
            opt = SLLI;
            break;
        case 0b101:
            // 再看FUNCT7
            switch((inst)>>25 & 0b1111111)
            {
            case 0b0000000:
                opt = SRLI;
                break;
            case 0b0100000:
                opt = SRAI;
                break;
            }
            break;
        case 0b010:
            opt = SLTI;
            break;
        case 0b011:
            opt = SLTIU;
            break;
        }
        break;
    case 0b0110111:
        opt = LUI;
        break;
    case 0b0010111:
        opt = AUIPC;
        break;
    case 0b1100011:
        // 再看 FUNCT 3
        switch((inst>>12) & 0b111)
        {
        case 0:
            opt = BEQ;
            break;
        case 0b001:
            opt = BNE;
            break;
        case 0b100:
            opt = BLT;
            break;
        case 0b101:
            opt = BGE;
            break;
        case 0b110:
            opt = BLTU;
            break;
        case 0b111:
            opt = BGEU;
            break;
        }
        break;
    case 0b1101111:
        opt = JAL;
        break;
    case 0b1100111:
        opt = JALR;
        break;
    default:
        //printf("instruction %08x fetching failed\n", inst);
        opt = NOP;
        break;
    }
}
void ALU(int inst, INSTRUCTION op)
{
    int tmp;
    switch(op){
    // 只是算出有效地址, x[rs1] + offset
    case LB:
    case LH:
    case LW:
    case LBU:
    case LHU:
        
    // 只是算出有效地址, x[rs1] + offset
    case SB:
    case SH:
    case SW:
        EX_MEM.ALUo = ID_EX.A + ID_EX.imm;  break;
        
    // 寄存器--立即数 ALU
    case ADDI:
        EX_MEM.ALUo = ID_EX.A + ID_EX.imm;  break;
    case LUI:
        EX_MEM.ALUo = ID_EX.imm;            break;
    case AUIPC:
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;         break;
    case XORI:
        EX_MEM.ALUo = ID_EX.A ^ ID_EX.imm;  break;
    case ORI:
        EX_MEM.ALUo = ID_EX.A | ID_EX.imm;  break;
    case ANDI:
        EX_MEM.ALUo = ID_EX.A & ID_EX.imm;  break;
    case SLLI:
        EX_MEM.ALUo = ID_EX.A << (ID_EX.imm & 0b11111);  break;
    case SRLI:
        tmp = ID_EX.imm & 0b11111;
        EX_MEM.ALUo = ID_EX.A>>tmp & (1<<32-tmp)-1;      break; 
    case SRAI:
        EX_MEM.ALUo = ID_EX.A >> (ID_EX.imm & 0b11111);  break;
    case SLTI:
        EX_MEM.ALUo = (ID_EX.A < ID_EX.imm)? 1: 0;       break;
    case SLTIU:
        EX_MEM.ALUo = ((unsigned int)ID_EX.A < (unsigned int)ID_EX.imm)? 1: 0;  break;
        
    // 寄存器--寄存器 ALU
    case ADD:
        EX_MEM.ALUo = ID_EX.A + ID_EX.B;  break;
    case SUB:
        EX_MEM.ALUo = ID_EX.A - ID_EX.B;  break;
    case XOR:
        EX_MEM.ALUo = ID_EX.A ^ ID_EX.B;  break;
    case OR:
        EX_MEM.ALUo = ID_EX.A | ID_EX.B;  break;
    case AND:
        EX_MEM.ALUo = ID_EX.A & ID_EX.B;  break;
    case SLL:
        EX_MEM.ALUo = ID_EX.A << (ID_EX.B & 0b11111);  break;
    case SRL:
        tmp = ID_EX.B & 0b11111;
        EX_MEM.ALUo = ID_EX.A>>tmp & (1<<32-tmp)-1;    break;
    case SRA:   
        EX_MEM.ALUo = ID_EX.A >> (ID_EX.B & 0b11111);  break;
    case SLT:
        EX_MEM.ALUo = (ID_EX.A < ID_EX.B)? 1: 0;       break;
    case SLTU:
        EX_MEM.ALUo = ((unsigned int)ID_EX.A < (unsigned int)ID_EX.B)? 1: 0;  break;
        
    // 控制指令,也只算出有效地址
    case BEQ:
        EX_MEM.cond = ID_EX.A == ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case BNE:
        EX_MEM.cond = ID_EX.A != ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case BLT:
        EX_MEM.cond = ID_EX.A < ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case BGE:
        EX_MEM.cond = ID_EX.A >= ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case BLTU:
        EX_MEM.cond = (unsigned int)ID_EX.A < (unsigned int)ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case BGEU:
        EX_MEM.cond = (unsigned int)ID_EX.A >= (unsigned int)ID_EX.B;
        EX_MEM.ALUo = ID_EX.NPC + ID_EX.imm - 4;  break;
    case JALR:
        EX_MEM.cond = true;  // 一元 -> 加减 -> 移位 -> and -> 赋值
        EX_MEM.ALUo = ID_EX.A+ID_EX.imm & ~1;     break;
    // 把JAL拿掉, JALR仍然去预测
    }
}
void load_store(int inst, INSTRUCTION op)
{
    unsigned int tmp;
    int rhs, t1, t2;
    switch(op){
    // load
    case LB:
        tmp = EX_MEM.ALUo;
        // char变int自动 按符号位 扩展高位
        MEM_WB.LMD = Memory[tmp];   break;
    case LH:
        tmp = EX_MEM.ALUo;
        t1 = Memory[tmp], t2 = Memory[tmp+1];
        t1 &= (1<<8)-1;
        MEM_WB.LMD = t1 + (t2<<8);  break;
    case LW:
        tmp = EX_MEM.ALUo;
        rhs = 0;
        for(int i=3; i>=0; --i){
            rhs <<= 8;
            rhs += (int)Memory[tmp+i] & (1<<8)-1;
        }
        MEM_WB.LMD = rhs;           break;
    case LBU:
        tmp = EX_MEM.ALUo;
        // unsigned 零扩展 扩展高位
        MEM_WB.LMD = Memory[tmp] & (1<<8)-1;
                                    break;
    case LHU:
        tmp = EX_MEM.ALUo;
        t1 = Memory[tmp], t2 = Memory[tmp+1];
        t1 &= (1<<8)-1, t2 &= (1<<8)-1;
        MEM_WB.LMD = t1 + (t2<<8);  break;
    
    // store
    case SB:
        tmp = EX_MEM.ALUo;
        rhs = EX_MEM.B & (1<<8)-1;
        Memory[tmp] = char(rhs);       break;
    case SH:
        tmp = EX_MEM.ALUo;
        rhs = EX_MEM.B & (1<<16)-1;
        Memory[tmp] = char(rhs);
        Memory[tmp+1] = char(rhs>>8);  break;
    case SW:
        tmp = EX_MEM.ALUo;
        rhs = EX_MEM.B;
        for(int i=0; i<4; ++i)  Memory[tmp+i] = char(rhs >> (8*i));
                                       break;
    }
}
void write_rd(int inst, INSTRUCTION op)
{
    switch(op){
    case LB:
    case LH:
    case LW:
    case LBU:
    case LHU:
        Register[inst>>7 & 0b11111] = MEM_WB.LMD;  break;
        
    case ADDI:
    case LUI:
    case AUIPC:
    case XORI:
    case ORI:
    case ANDI:     
    case SLLI:
    case SRLI:
    case SRAI:   
    case SLTI:
    case SLTIU:
    
    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:  
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:
        Register[inst>>7 & 0b11111] = MEM_WB.ALUo;  break;
    };
    // 别忘了硬件低电平!
    Register[0] = 0;
    
}
bool bypass1()
{
    int inst1 = EX_MEM.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return false;
    INSTRUCTION op1=type[1], op2=type[0];
    int rd = inst1>>7 & 0b11111, rs1, rs2(-1);
    switch(op2){
    // 第i+1句要有源寄存器
    case JALR: 
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
    case ADDI:  
    case XORI: 
    case ORI:   
    case ANDI:  
    case SLLI:  
    case SRLI:  
    case SRAI:  
    case SLTI:  
    case SLTIU: 
        rs1 = inst2>>15 & 0b11111; break;

    case SB:
    case SH:
    case SW:

    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:

    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        rs1 = inst2>>15 & 0b11111;
        rs2 = inst2>>20 & 0b11111; break;

    default:
        return false;
    };
    switch(op1){ // 第i句必须有目的寄存器
    case ADDI:
    case LUI:
    case AUIPC:
    case XORI:
    case ORI:
    case ANDI:     
    case SLLI:
    case SRLI:
    case SRAI:   
    case SLTI:
    case SLTIU:
    
    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:  
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:
        if(rd == rs1)   ID_EX.A = EX_MEM.ALUo;
        if(rd == rs2)   ID_EX.B = EX_MEM.ALUo; 
        return false; 
    
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
        return (rd == rs1 || rd == rs2)? true: false;

    default:
        return false;
    };
}
void bypass2()
{
    int inst1 = MEM_WB.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return;
    INSTRUCTION op1=type[2], op2=type[0];
    int rd = inst1>>7 & 0b11111;
    int rs1, rs2(-1);
    switch(op2){ // 第i+1句指令必须要有源寄存器
    case JALR: 
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
    case ADDI:  
    case XORI: 
    case ORI:   
    case ANDI:  
    case SLLI:  
    case SRLI:  
    case SRAI:  
    case SLTI:  
    case SLTIU: 
        rs1 = inst2>>15 & 0b11111;
        if(stall1)
            if(rd == rs1)   ID_EX.A = Register[rd];
        break;

    case SB:
    case SH:
    case SW:

    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:

    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        rs1 = inst2>>15 & 0b11111;
        rs2 = inst2>>20 & 0b11111;
        if(stall1){
            if(rd == rs1)   ID_EX.A = Register[rd];
            if(rd == rs2)   ID_EX.B = Register[rd];
        }
        break;
    
    default:
        return;
    }
    switch(op1){ // 第i句必须有目的寄存器
    case ADDI:
    case LUI:
    case AUIPC:
    case XORI:
    case ORI:
    case ANDI:     
    case SLLI:
    case SRLI:
    case SRAI:   
    case SLTI:
    case SLTIU:
    
    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:  
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:
        if(rd == rs1)   ID_EX.A = MEM_WB.ALUo;
        if(rd == rs2)   ID_EX.B = MEM_WB.ALUo;
        return;

    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
        if(rd == rs1)   ID_EX.A = MEM_WB.LMD;
        if(rd == rs2)   ID_EX.B = MEM_WB.LMD;        
        return;

    default:
        return;
    };
}
bool BTB_write()
{
    int inst = ID_EX.IR;
    if(!inst)   return false; // 处理没有信号的情况
    INSTRUCTION op=type[0];
    switch(op){
    // 小心改地址这件事情
    // 真的要注意改地址这件事情
    // 多一个改地址的事情
    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:

    case JALR:
        if(EX_MEM.cond) {
            if(BTB[ID_EX.NPC-4] != EX_MEM.ALUo){
                BTB[ID_EX.NPC-4] = EX_MEM.ALUo;
                pred_branch = false;
                ++n;    return true; 
            }
            else{ 
                pred_branch = true;
                ++y;  return false;
            }
        }
        else {
            if(BTB[ID_EX.NPC-4]){
                PC = ID_EX.NPC;
                BTB[ID_EX.NPC-4] = 0;
                ++n;     return true;
            } 
            else
                ++y;    return false;
        } 
          
    default:
        return false;
    };
}
void ADDer(int inst)
{
    Register[inst>>7 & 0b11111] = IF_ID.NPC;    Register[0] = 0;
    PC = ID_EX.NPC + ID_EX.imm - 4;
    ID_EX.clear();
    return;
}
// Instruction Fetch
// 完成 1) 指令生成 2) pc + 4
int inst_fetch()
{
    int inst = 0;
    for(int i=0; i<4; ++i)  
        inst += ((int)Memory[PC+i] & 0b11111111) << 8*i;
    IF_ID.IR = inst,  IF_ID.NPC = PC+4;
    // 通过判断这个PC是否是BTB里面的一个Key预测是否跳转
    if(BTB[PC])
        PC = BTB[PC];
    else
        PC += 4;
    return inst;
} 
// Instruction Decode
// 完成 1) 译码 2) 立即数扩展
void inst_decode()
{
    int inst = IF_ID.IR;
    if(!inst)   return; // 处理没有信号的情况
    INSTRUCTION op;
    visit_IR(inst, op);
    int imm;
    int imm1, imm2, imm3, imm4;
    // 完成立即数扩展和寄存器访问
    switch(op){
    // I-type
    case JALR:
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
    case ADDI:  
    case XORI: 
    case ORI:   
    case ANDI:  
    case SLLI:  
    case SRLI:  
    case SRAI:  
    case SLTI:  
    case SLTIU: 
        ID_EX.A = Register[inst>>15 & 0b11111];
        imm = inst>>20 & (1<<12)-1;
        if(inst < 0)    imm += (1<<20)-1 << 12;
        ID_EX.imm = imm;   break; 

    // S-type
    case SB:
    case SH:
    case SW:
        ID_EX.A = Register[inst>>15 & 0b11111];
        ID_EX.B = Register[inst>>20 & 0b11111];
        imm = ((inst>>25 & 0b1111111) << 5) + (inst>>7 & 0b11111);
        if(inst < 0)    imm += (1<<20)-1 << 12;
        ID_EX.imm = imm;   break;

    // R-type
    case ADD:
    case SUB:
    case XOR:
    case OR:
    case AND:
    case SLL:
    case SRL:
    case SRA:
    case SLT:
    case SLTU:
        ID_EX.A = Register[inst>>15 & 0b11111];
        ID_EX.B = Register[inst>>20 & 0b11111]; break;

    // B-type
    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        ID_EX.A = Register[inst>>15 & 0b11111];
        ID_EX.B = Register[inst>>20 & 0b11111];
        imm1 = inst>>8 & 0b1111;         // imm[4:1]
        imm2 = (inst>>25 & 0b111111)<<4; // imm[10:5]
        imm3 = (inst>>7  & 1)<<10;       // imm[11]
        imm4 = (inst>>31 & 1)<<11;       // imm[12]
        imm = imm1+imm2+imm3+imm4;       // imm[12:1] 不过是低位的
        if(inst < 0)    imm += (1<<20)-1 << 12;
        imm <<= 1;
        ID_EX.imm = imm;  break;

    // U-type
    case LUI:
    case AUIPC:
        imm = inst>>12 & (1<<20)-1;
        imm <<= 12;
        ID_EX.imm = imm;  break;

    // J-type
    case JAL:
        imm1 = inst>>21 & (1<<10)-1;     // imm[10:1]
        imm2 = (inst>>20 & 1)<<10;       // imm[11]
        imm3 = (inst>>12 & (1<<8)-1)<<11;// imm[19:12]
        imm4 = (inst>>31 & 1)<<19;       // imm[20]
        imm = imm1+imm2+imm3+imm4;       // imm[20:1] 不过是低位的
        if(inst < 0)    imm += (1<<12)-1 << 20;
        imm <<= 1;
        ID_EX.imm = imm;  break;
    }
    ID_EX.IR = IF_ID.IR,  ID_EX.NPC = IF_ID.NPC;
    type[0] = op;
    if(op == JAL)    ADDer(inst);
}
// Execution
// ALU performs one of three functions
// 1) Memory Reference ---- base register + offset to form the effective Address
// 2) Register -- Register ALU instruction
// 3) Register -- Immediate ALU instruction 
void execute()
{
    int inst = ID_EX.IR;
    if(!inst)   return; // 处理没有信号的情况
    INSTRUCTION op = type[0];
    ALU(inst, op);
    EX_MEM.IR = ID_EX.IR, EX_MEM.B = ID_EX.B;
    type[1] = op;
}
// Memory Access
// 如果是分支指令, cond成功 则将转移目标地址送给PC
// 如果是内存读写指令, 就读写
void mem_access()
{
    int inst = EX_MEM.IR;
    if(!inst)   return; // 处理没有信号的情况
    INSTRUCTION op = type[1];
    load_store(inst, op);
    if(EX_MEM.cond && !pred_branch)  PC = EX_MEM.ALUo;
    MEM_WB.ALUo = EX_MEM.ALUo,  MEM_WB.IR = EX_MEM.IR; 
    type[2] = op;
}
// Write Back
// load 指令写回寄存器
// 寄存器 -- 寄存器 指令写回寄存器
void write_back()
{
    int inst = MEM_WB.IR;
    if(!inst)   return;  // 处理没有信号的情况
    INSTRUCTION op = type[2];
    write_rd(inst, op);
}

// file IO 函数区
void get_into_mem()
{
    // 现在需要考虑的问题是
    if(tmp[0]>47 && tmp[0]<58)
        tmp[0] -= 48;
    else 
        tmp[0] -= 55;
    if(tmp[1]>47 && tmp[1]<58)
        tmp[1] -= 48;
    else
        tmp[1] -= 55;
    Memory[RW] = (tmp[0]<<4) + tmp[1];
    ++RW;
}
void locate_rw()
{
    int i = 1;
    unsigned int posi = 0;
    for(; i<9; ++i){
        if(tmp[i]>47 && tmp[i]<58) // 是数字的ASCII码
            tmp[i] -= 48;
        else                       // 是大写字母的ASCII码
            tmp[i] -= 55;
        posi <<= 4;
        posi += tmp[i];
    }
    RW = posi; 
}

int main()
{
    // file IO for fake memory
    while(scanf("%s",tmp) != EOF){
        if(tmp[0] == '@')   locate_rw();
        else             get_into_mem();
    }

    // simulation start!
    // 5 stages pipeline with hazard detection
    PC = 0;
    while(true){
    // Write back
        bypass2();
        write_back();
        MEM_WB.clear();
    // Memory access
        stall1 = bypass1();
        mem_access();
        EX_MEM.clear();   
        if(stall1)   continue;
    // Execution
        execute();
        flush2 = BTB_write();
        ID_EX.clear();
        if(flush2){
            IF_ID.clear();  inst = 0;
            continue;
        }
    // Instruction decode
        inst_decode();
        IF_ID.clear();
        if(inst == 0x0ff00513)  break; 
    // Instruction fetch
        inst = inst_fetch();
    }

    // output result in a0 register
    printf("%d\n", (unsigned int)Register[10] & 255);
    printf("right predicted: %d, wrong predicted: %d\t", y, n);
    printf("prediction accuracy: %f\n", double(y)/(y+n));
}