#include <iostream>
#include <stdio.h>
#include <map>

// 为了简便开的一些全局变量
int Register[32] = {0}, IR;
unsigned int PC, RW, y(0), n(0);
char Memory[0x100000] = {0};
FILE *fp;
char tmp[10];
bool pred_branch;
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
    void clear(){
        IR = NPC = 0;
    }
}IF_ID;
struct pipeline_register2{
    int NPC, A, B, imm, IR;
    void clear(){
        NPC = A = B = imm = IR = 0;
    }
}ID_EX;
struct pipeline_register3{
    int ALUo, B, IR;
    bool cond; // 判断分支是否成功
    void clear(){
        ALUo = B = IR = cond = 0;
    }
}EX_MEM;
struct pipeline_register4{
    int ALUo, LMD, IR;
    void clear(){
        ALUo = LMD = IR = 0;
    }
}MEM_WB;

// branch target buffer
std::map<int, int>BTB;


// 先写辅助函数,模拟一个器件
void visit_reg_file(int inst, INSTRUCTION op)
{
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
        ID_EX.A = Register[inst>>15 & 0b11111]; break;

    // S-type
    case SB:
    case SH:
    case SW:

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

    // B-type
    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        ID_EX.A = Register[inst>>15 & 0b11111];
        ID_EX.B = Register[inst>>20 & 0b11111]; break;

    // U-type
    case LUI:
    case AUIPC:

    // J-type
    case JAL:  break;
    }
}
void imm_extend(int inst, INSTRUCTION op)
{
    int imm;
    int imm1, imm2, imm3, imm4;
    // 注意,哪怕后面跟unsigned的都是先按照符号位扩展,扩展完了再去unsigned
    switch(op){

    // I-type 得到imm[11:0]
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
        imm = inst>>20 & (1<<12)-1;
        if(inst < 0)    imm += (1<<20)-1 << 12;
        ID_EX.imm = imm;   break; 
        
    // S-type 得到imm[11:0]
    case SB:
    case SH:
    case SW:
        imm = ((inst>>25 & 0b1111111) << 5) + (inst>>7 & 0b11111);
        if(inst < 0)    imm += (1<<20)-1 << 12;
        ID_EX.imm = imm;   break;
        
    // R-type
    case ADD:    
    case SUB:    
    case SLL:    
    case SLT:    
    case SLTU:   
    case XOR:    
    case OR:     
    case AND:    
    case SRL:    
    case SRA:             break;

    // U-type 得到imm[31:12]
    case LUI:
    case AUIPC:
        imm = inst>>12 & (1<<20)-1;
        imm <<= 12;
        ID_EX.imm = imm;  break;
        
    // B-type 得到imm[12:1]
    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        imm1 = inst>>8 & 0b1111;         // imm[4:1]
        imm2 = (inst>>25 & 0b111111)<<4; // imm[10:5]
        imm3 = (inst>>7  & 1)<<10;       // imm[11]
        imm4 = (inst>>31 & 1)<<11;       // imm[12]
        imm = imm1+imm2+imm3+imm4;       // imm[12:1] 不过是低位的
        if(inst < 0)    imm += (1<<20)-1 << 12;
        imm <<= 1;
        ID_EX.imm = imm;  break;
        
    // J-type 得到imm[20:1]
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
}
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
    // 把JAL和 JALR拿掉
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
void bypass1_ALU()
{
    int inst1 = EX_MEM.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return;
    INSTRUCTION op1=type[1], op2=type[0];
    int rd, rs1, rs2(-1);
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

    // 抹掉 JAL和 JALR指令
        rd = inst1>>7 & 0b11111;    break;
    
    default:
        return;
    };
    switch(op2){ // 第i+1句必须有源寄存器
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
        rs1 = inst2>>15 & 0b11111;  break;

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
        rs2 = inst2>>20 & 0b11111;  break;
    
    default:
        return;
    }
    if(rd == rs1)   ID_EX.A = EX_MEM.ALUo;
    if(rd == rs2)   ID_EX.B = EX_MEM.ALUo;     
}
bool bypass1_load()
{
    int inst1 = EX_MEM.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return false;
    INSTRUCTION op1=type[1], op2=type[0];
    int rd, rs1, rs2(-1);
    switch(op1){
    // 第i句必须是load
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
        rd = inst1>>7 & 0b11111;    break;

    default:
        return false;
    };
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
    // 第i句的目的寄存器是第i+1句的源寄存器
    return (rd == rs1 || rd == rs2)? true: false;
}
void bypass2_ALU()
{
    int inst1 = MEM_WB.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return;
    INSTRUCTION op1=type[2], op2=type[0];
    int rd, rs1, rs2(-1);
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
        rd = inst1>>7 & 0b11111;    break;

    default:
        return;
    };
    switch(op2){ // 第i+2句必须有源寄存器
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
        rs1 = inst2>>15 & 0b11111;  break;

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
        rs2 = inst2>>20 & 0b11111;  break;
    
    default:
        return;
    }
    // 想一下这咋办
    // 上一个clock cycle,当前指令在MEM,如果是JAL,已经写了目的寄存器
    // 而下两条指令是在ID,由于MEM在ID的上面,所以ID取到的就是目的寄存器的链接结果
    // 所以这个时候当前指令是JAL/JALR不会影响源寄存器的读写
    if(rd == rs1)   ID_EX.A = MEM_WB.ALUo;
    if(rd == rs2)   ID_EX.B = MEM_WB.ALUo;
}
void bypass1_store()
{
    int inst1 = MEM_WB.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return;
    INSTRUCTION op1=type[2], op2=type[0];
    int rd, rs2;
    switch(op1){
    // 第i句只能是Load
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
        rd = inst1>>7 & 0b11111;    break;

    default:
        return;
    };
    switch(op2){
    // 第i+1句只能是store, 因为只有store指令的 clock cycle不为负
    case SB:
    case SH:
    case SW:
        rs2 = inst2>>20 & 0b11111;  break;
    
    default:
        return;
    }
    // 尽量模拟真实情况,因为还没WB呢
    if(rd == rs2)   ID_EX.B = MEM_WB.LMD;
}
void bypass2_load()
{
    int inst1 = MEM_WB.IR, inst2 = ID_EX.IR;
    if(!inst1 || !inst2)    return;
    INSTRUCTION op1=type[2], op2=type[0];
    int rd, rs1, rs2(-1);
    switch(op1){ // 第i句指令只能是load
    case LB:    
    case LH:    
    case LW:    
    case LBU:   
    case LHU:   
        rd = inst1>>7 & 0b11111;    break;

    default:
        return;
    };
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
        rs1 = inst2>>15 & 0b11111;  break;

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
        rs2 = inst2>>20 & 0b11111;  break;
    
    default:
        return;
    }
    // 尽量模拟真实情况
    if(rd == rs1)   ID_EX.A = MEM_WB.LMD;
    if(rd == rs2)   ID_EX.B = MEM_WB.LMD;
}
void bypass1_WB(bool stall1)
{
    if(stall1){
        int inst1 = MEM_WB.IR, inst2 = ID_EX.IR;
        int rd = inst1>>7 & 0b11111;
        int rs1, rs2(-1);
        INSTRUCTION op2=type[0];
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
            return;
        };
        // 这个可以,因为WB在下一个clock cycle的first half完成
        if(rd == rs1)   ID_EX.A = Register[rd];
        if(rd == rs2)   ID_EX.B = Register[rd];
    }
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

        if(EX_MEM.cond){
            std::pair<std::map<int, int>::iterator, int> tmp;
            tmp = BTB.insert(std::pair<int, int>(ID_EX.NPC - 4, EX_MEM.ALUo));
            if(tmp.second){ // BTB没有,但是要branch,应该加一个字段,需要一个flush
                pred_branch = false;
                ++n;    return true;
            }
            else{ // BTB有,而且需要branch
            // 多一个改地址的事情
            // 如果当时预测的跳转地址发生了变化,仍然是预测失败,在这里先修改BTB,在下一个时钟周期的MEM修改PC
                if(tmp.first->second != EX_MEM.ALUo){
                    tmp.first->second = EX_MEM.ALUo;
                    pred_branch = false;
                    ++n;    return true;
                }   
                else{
                    pred_branch = true;
                    ++y;    return false;
                }
            }
        }
        else  
            if(BTB.erase(ID_EX.NPC - 4)){ // BTB有,但是不要branch,应该删一个字段,需要一个flush
                // "拨乱反正", 即重新设PC
                // “拨乱反正” 这个地方是错的,但是目标是达到白取第二条指令的目的
                PC = ID_EX.NPC - 4;
                ++n;    return true;
            }
            else{ // BTB没有而且不要branch
                ++y;    return false;
            }
    
    default:    // 如果BTB没有且不是branch,没有任何影响,直接return掉了
        return false;
    };
}
void ADDer(int inst, INSTRUCTION op)
{
    if(op == JAL){
        Register[inst>>7 & 0b11111] = IF_ID.NPC;    Register[0] = 0;
        PC = ID_EX.NPC + ID_EX.imm - 4;
        ID_EX.clear();
        return;
    }
    if(op == JALR){
        Register[inst>>7 & 0b11111] = IF_ID.NPC;    Register[0] = 0;
        PC = ID_EX.A + ID_EX.imm & ~1;
        ID_EX.clear();
        return;
    }
}
// Instruction Fetch
// 完成 1) 指令生成 2) pc + 4
int inst_fetch()
{
    int inst = 0;
    // 我们发现,每一个负码后面都会乱一个！
    for(int i=0; i<4; ++i)  
        inst += ((int)Memory[PC+i] & 0b11111111) << 8*i;
    IF_ID.IR = inst,  IF_ID.NPC = PC+4;
    // 通过判断这个PC是否是BTB里面的一个Key预测是否跳转
    if(BTB.count(PC))
        PC = BTB.find(PC)->second;
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
    visit_reg_file(inst, op);
    imm_extend(inst, op);
    ID_EX.IR = IF_ID.IR,    ID_EX.NPC = IF_ID.NPC;
    type[0] = op;
    ADDer(inst, op);
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
    EX_MEM.IR = ID_EX.IR;
    // 这个B只服务于store指令
    EX_MEM.B = ID_EX.B;
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
    if(EX_MEM.cond) {  
        // 一定要注意, 要跳转且预测成功的话, PC会被重新set一次
        // 预测正确就不用set了呗, 预测不跳才branch
        // 这个pred_branch是上一步写下来的
        if(!pred_branch) PC = EX_MEM.ALUo;
    }
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
    if(tmp[0]>64 && tmp[0]<71)
        tmp[0] -= 55;
    if(tmp[1]>47 && tmp[1]<58)
        tmp[1] -= 48;
    if(tmp[1]>64 && tmp[1]<71)
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
        if(tmp[i]>64 && tmp[i]<71) // 是大写字母的ASCII码
            tmp[i] -= 55;
        posi <<= 4;
        posi += tmp[i];
    }
    RW = posi; 
}

int main()
{
    // file IO for fake memory
    fp = fopen("testcase/superloop.data", "r");
    while(fscanf(fp,"%s",tmp) != EOF){
        if(tmp[0] == '@')   locate_rw();
        else             get_into_mem();
    }

    // simulation start!
    // 5 stages pipeline with hazard detection
    // 有一步Load失败了
    PC = 0;
    int inst(0);
    bool stall1 = false, flush2 = false;
    while(true){
    // Write back
        bypass2_ALU();
        bypass2_load();
        bypass1_store();
        write_back();
        bypass1_WB(stall1);
        MEM_WB.clear();
    // Memory access
        bypass1_ALU();
        stall1 = bypass1_load();
        mem_access();
        EX_MEM.clear();   
        if(stall1)   continue;
    // Execution
        execute();
        flush2 = BTB_write();
        ID_EX.clear();
        if(flush2){
            IF_ID.clear();
            inst = 0;
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
