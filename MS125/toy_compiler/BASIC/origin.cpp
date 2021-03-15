#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <stack>
#include <queue>

// -------- lexer --------- //


enum tok_type { // TokenType
    tok_eof = -1,
    // commands
    tok_let = -2,
    tok_input = -3,
    tok_exit = -4,
    tok_goto = -5,
    tok_if = -6,
    tok_then = -7,
    tok_for = -8,
    tok_end = -9,
    tok_op = -10,
    tok_semi = -11,
    tok_komm = -12,
    tok_lparend = -13,
    tok_rparend = -14,
    tok_line_break = -15,
    // primary
    tok_identifier = -16,
    tok_number = -17
};
enum Operator { // binary operands
	ADD, MINUS, PROD, DIV, 
	AND, OR, 
	EQL, NEQ, GE, LE, GRT, LESS,
    ASSIGN
};

std::string IdentifierStr; // Filled in if tok_identifier
int NumVal;                // Filled in if tok_number
int Curline;               // show current line number

struct Token{
    tok_type type;
    int line;
    // Attribute value
    std::string name;
    int value;
    Operator op;
    Token() {}
    Token(tok_type t): type(t)  { line = Curline; }
    Token(tok_type t, int val): type(t), value(val)     { line = Curline; }
    Token(tok_type t, std::string n): type(t), name(n)  { line = Curline; }
    Token(tok_type t, Operator OP): type(t), op(OP)     { line = Curline; }
};

Token gettok() {
    char LastChar = ' ';

    // Skip any whitespace.
    while (isblank(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) {     // identifier:[a-z,A-Z] and keywords:[A-Z]
        IdentifierStr = LastChar;
        while (isalpha((LastChar = getchar())))
            IdentifierStr += LastChar;
        ungetc(LastChar, stdin); // 吐回去一个

        // comment until the end of line
        if(IdentifierStr == "REM") {
            do  LastChar = getchar();
            while (LastChar != EOF && LastChar != '\n'); // Windows系统里真正的是 \r\n

            if (LastChar != EOF) {
				LastChar = getchar();
				if(LastChar == EOF)
					return Token(tok_eof);
				ungetc(LastChar, stdin);
                std::cin >> Curline;
                return gettok();
            }
            // 似乎ctrl+Z只能在下一行
            else
                return Token(tok_eof);
        }
        // keywords
        if(IdentifierStr == "LET")      return Token(tok_let);
        if(IdentifierStr == "INPUT")    return Token(tok_input);
        if(IdentifierStr == "EXIT")     return Token(tok_exit);
        if(IdentifierStr == "GOTO")     return Token(tok_goto);
        if(IdentifierStr == "IF")       return Token(tok_if);
        if(IdentifierStr == "THEN")     return Token(tok_then);
        if(IdentifierStr == "FOR")      return Token(tok_for);
        if(IdentifierStr == "END")      return Token(tok_end);
        // identifiers
        return Token(tok_identifier, IdentifierStr);
    }
    if(isdigit(LastChar)) {   // Number:[0-9], 因为BASIC全部是int
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar));
        ungetc(LastChar, stdin); // 问题在这里! 每次多吃了一个

        NumVal = std::stoi(NumStr);
        return Token(tok_number, NumVal);
    }

    // Check for end of file.  Don't eat the EOF.
    if(LastChar == EOF)    return Token(tok_eof);
    // process the line number
    if(LastChar == '\n') {
		LastChar = getchar();
		if(LastChar == EOF)
			return Token(tok_eof);
		ungetc(LastChar, stdin);
        std::cin >> Curline;
        return Token(tok_line_break);
    }

    // Otherwise, 由于我们有不止一位的运算符, 所以不能直接返回ascii码
    // +-*/, && || !=, == < > <= >=, 以及 ; = \n
    // 这块可能有问题啊,这个地方乱套了
    char ThisChar = LastChar;
    // 先处理只有一位的运算符
    switch(ThisChar){
        case '+':   return Token(tok_op, ADD);
        case '-':   return Token(tok_op, MINUS);
        case '*':   return Token(tok_op, PROD);
        case '/':   return Token(tok_op, DIV);
        case ';':   return Token(tok_semi);
        case '(':   return Token(tok_lparend);
        case ')':   return Token(tok_rparend);
        case ',':   return Token(tok_komm);
        case '!':   
            LastChar = getchar();
            return Token(tok_op, NEQ);
    }
    LastChar = getchar();
    if(ThisChar == LastChar)
        switch(ThisChar){
            case '=': return Token(tok_op, EQL);
            case '&': return Token(tok_op, AND);
            case '|': return Token(tok_op, OR);
        }
    else {
        if(ThisChar == '=') {
            ungetc(LastChar, stdin);
            return Token(tok_op, ASSIGN);
        }
        if(ThisChar == '<')
            if(LastChar == '=') 
                return Token(tok_op, LE);
            else {
                ungetc(LastChar, stdin);
                return Token(tok_op, LESS);
            }               
        if(ThisChar == '>')     
            if(LastChar == '=') 
                return Token(tok_op, GE);
            else {
                ungetc(LastChar, stdin);
                return Token(tok_op, GRT);
            }
    }
}

std::ostream &operator<<(std::ostream &_os, Token _tok) {
    putchar('<');
    switch(_tok.type){
    case tok_eof:
        return std::cout << "EOF>";
    case tok_let:
        return std::cout << "LET>";
    case tok_input:
        return std::cout << "INPUT>";
    case tok_exit:
        return std::cout << "EXIT>";
    case tok_goto:
        return std::cout << "GOTO>";
    case tok_if:
        return std::cout << "IF>";
    case tok_then:
        return std::cout << "THEN>";
    case tok_for:
        return std::cout << "FOR>";
    case tok_end:
        return std::cout << "END>";
    case tok_op:
        switch(_tok.op){
        case ADD:
            return std::cout << "+>";
        case MINUS:
            return std::cout << "->";
        case PROD:
            return std::cout << "*>";
        case DIV:
            return std::cout << "/>";
        case AND:
            return std::cout << "&&>";
        case OR:
            return std::cout << "||>";
        case EQL:
            return std::cout << "==>";
        case NEQ:
            return std::cout << "!=>";
        case GE:
            return std::cout << ">=>";
        case LE:
            return std::cout << "<=>";
        case GRT:
            return std::cout << ">>";
        case LESS:
            return std::cout << "<>";
        case ASSIGN:
            return std::cout << "=>";
        }
    case tok_semi:
        return std::cout << ";>";
    case tok_komm:
        return std::cout << ",>";
    case tok_lparend:
        return std::cout << "(>";
    case tok_rparend:
        return std::cout << ")>";
    case tok_line_break:
        return std::cout << "LINE_BREAK>\n";
    case tok_identifier:
        return std::cout << "id," << _tok.name << '>';
    case tok_number:
        return std::cout << "num," << _tok.value << '>';
    }
    return std::cout;
}
std::ostream &operator<<(std::ostream &_os, Operator _op){
    switch(_op){
        case ADD:
            return std::cout << " + ";
        case MINUS:
            return std::cout << " - ";
        case PROD:
            return std::cout << " * ";
        case DIV:
            return std::cout << " / ";
        case AND:
            return std::cout << "&& ";
        case OR:
            return std::cout << "|| ";
        case EQL:
            return std::cout << "== ";
        case NEQ:
            return std::cout << "!= ";
        case GE:
            return std::cout << ">= ";
        case LE:
            return std::cout << "<= ";
        case GRT:
            return std::cout << " > ";
        case LESS:
            return std::cout << " < ";
        case ASSIGN:
            return std::cout << " = ";
        };
	return std::cout;
}
std::string optos(Operator _op) {
    switch(_op){
        case ADD:
            return std::string("+");
        case MINUS:
            return std::string("-");
        case PROD:
            return std::string("*");
        case DIV:
            return std::string("/");
        case AND:
            return std::string("&&");
        case OR:
            return std::string("||");
        case EQL:
            return std::string("==");
        case NEQ:
            return std::string("!=");
        case GE:
            return std::string(">=");
        case LE:
            return std::string("<=");
        case GRT:
            return std::string(">");
        case LESS:
            return std::string("<");
        case ASSIGN:
            return std::string("=");
        };
    return std::string(" ");
}

class LEXER { 
	int i;
public:
	std::vector<Token> text;
	LEXER(): i(0) {}
	void lexer()
	{
		Token LastTok;
		do {
        	LastTok = gettok();
        	text.push_back(LastTok);
    	}	
		while(LastTok.type != tok_eof);
	}
	Token getNextToken() { return text[i++];} 
}Lexer;


// ------- Instruction Selector ------- //
int CurTmp;   // current tmp variety number
int CurBlock; // current Basicblock of 3AC, use index in CFG's
int TotBlock; // total Basicblock number

class Basicblock {
    int L;     // Block number
    std::string tmp;
public:
    std::vector<std::string> TAC;
    int head;  // first line of Block
    Basicblock() {}
    Basicblock(int HEAD): head(HEAD) { L = ++TotBlock; }
    bool empty()                 { return TAC.empty(); }  // any 3AC in this Block
    void add(std::string tac){ 
        // std::cout<<" !! "<<std::endl;
        tmp = tac;
         TAC.push_back(tmp); 
        }
    int num() { return L; }
    void demo()
    {
        std::cout << "Basicblock " << L << "   " << head << std::endl;
        for(int i=0; i < TAC.size(); ++i)
            std::cout << TAC[i] << std::endl;
    }
    Basicblock operator=(Basicblock rhs)
    {
        TAC = rhs.TAC;
        L = rhs.L;
        head = rhs.head;
        return *this;
    }
}trueBlock, falseBlock;
std::vector<Basicblock> CFG;
std::queue<Basicblock> Flow;
Basicblock searchBlock(int LINE) // 找到目标BASIC行号起头的Basicblock,否则返回一个新的Basicblock 
{
    int i = 0;
    while(i < CFG.size()) {
        if(CFG[i].head == LINE) break;
        else    ++i;
    }
    if(i == CFG.size()) {
        Basicblock tmpBlock(LINE);
        CFG.push_back(tmpBlock);
    }
    return CFG[i];
}
std::map<int, bool> CtrlTable; 

// ------- Abstract Syntax Tree ---------- //

namespace {

class ASTnode{
public:
	virtual ~ASTnode() = default;
};

class Expression: public ASTnode{
public:
	enum expr_type {
		expr_id,
		expr_num,
		expr_bin
	};
	expr_type type;
	virtual ~Expression() = default;
    virtual std::string translate() = 0;
	virtual void debug()
	{
		std::cout<<"!!!";
	}
};

class Identifier: public Expression{	
public:	// 弄上缺省值,预留以后的INPUT
	std::string name;
	int value;
	Identifier(std::string NAME, int VAL = 0): 
		name(NAME), value(VAL) { type = expr_id; }
	virtual void debug()
	{
		std::cout<<"("<<name<<"("<<value<<"))";
	}
    virtual std::string translate() 
    { 
        return name; 
    }
};

class NumValue: public Expression{
public:
	int num;
	NumValue(int n): num(n) { type = expr_num; }
	virtual void debug()
	{
		std::cout<<"("<<num<<")";
	}
    virtual std::string translate() 
    {
         return std::to_string(num); 
    }
};

class BinExpr: public Expression{
public:
	Operator op;
	Expression *lhs, *rhs;
  	BinExpr(Operator OP, Expression *LHS, Expression *RHS)
		:op(OP), lhs(LHS), rhs(RHS) { type = expr_bin; }
	virtual void debug()
	{
		std::cout<<"(  ";
		lhs->debug();
		rhs->debug();
		std::cout<<"  )";
	}
    virtual std::string translate()
    {
        std::string str1, str2, res;
        str1 = lhs->translate();
        str2 = rhs->translate();
        std::string rd("tmp");
        rd += std::to_string(CurTmp++);
        res = optos(op) + ' ' + str1 + ", " + str2 + ", " + rd; 
        CFG[CurBlock].add(res);
        return rd;
    }
};


class Statement: public ASTnode{
public:	
	enum stmt_type {
		stmt_var,
		stmt_input,
		stmt_if,
		stmt_goto,
		stmt_for, stmt_end,
		stmt_exit
	};
	int line;
	stmt_type type;
	virtual ~Statement() = default;
    virtual void translate() = 0;
	virtual void debug()
	{
		std::cout<<"!!!"<<std::endl;
	}
};

std::list<Statement*> Program;      // 虚假的树根
std::list<Statement*>::iterator IS; // 指令选择器, 计数BASIC里面的内容
void setIS(int LINE)                // 将IS移到BASIC相应的行号
{
    std::list<Statement*>:: iterator tmpIter;
    tmpIter = Program.begin();
    while(tmpIter != Program.end()) {
        if((*tmpIter)->line == LINE) {
            IS = tmpIter;
            return;
        }
        else   ++tmpIter;
    }
}

class VarStmt: public Statement{ 
	Identifier *lhs; 			 
	Expression *rhs;
public:
	VarStmt(int LINE, Identifier *LHS, Expression *RHS): 
		lhs(LHS), rhs(RHS) { line = LINE; type = stmt_var;}
	virtual void debug()
	{
		std::cout<<"LET"<<"  (  ";
		lhs->debug();
		rhs->debug();
		std::cout<<"  )"<<std::endl;
	}
    virtual void translate()
    {
        std::string str1, str2, res;
        str1 = lhs->translate();
        str2 = rhs->translate();
        res = str1 + " = " + str2; 
        CFG[CurBlock].add(res);
        return;
    }
};

class InputStmt: public Statement{
	std::vector<Identifier*> id;
public:
	InputStmt(int LINE, std::vector<Identifier*> ID): 
		id(ID) { line = LINE; type = stmt_input; }
	virtual void debug()
	{
		std::cout<<"INPUT"<<"  ";
		auto it = id.begin();
		while(it != id.end())
		{
			(*it)->debug();
			it++;
		}
		std::cout<<std::endl;
	}
    virtual void translate()
    {
        std::string res = "input ";
        res += id[0]->name;
        for(int i=1; i<id.size(); ++i)  
        {
            res += ',' + id[i]->name;
        }
        CFG[CurBlock].add(res);
        return;
    }
};

class GotoStmt: public Statement{
	int target;
public:
	GotoStmt(int LINE, int GOTO): 
		target(GOTO) { line = LINE; type = stmt_goto; }
	virtual void debug()
	{
		std::cout<<"GOTO"<<"  ";
		std::cout<<target;
		std::cout<<std::endl;
	}
    virtual void translate()
    {   // 找/辟出跳之后需要的的Basicblock
        trueBlock = searchBlock(target);
        // 原Basicblock里面添上跳转的3AC作为结尾
        std::string res = "goto Block";
        res += std::to_string(trueBlock.num());
        CFG[CurBlock].add(res);
        // 如果需要一个新的Basicblock,将其入队
        if(trueBlock.empty())
        {
            Flow.push(trueBlock);
        }
    }
};

class IfStmt: public Statement{
	Expression *cond;
	int target;
public:
	IfStmt(int LINE, Expression *EXPR, int THEN): 
		cond(EXPR), target(THEN) { line = LINE; type = stmt_if; }
	virtual void debug()
	{
		std::cout<<"IF"<<"  (  ";
		cond->debug();
		std::cout<<"  )  "<<target<<std::endl;
	}
    virtual void translate()
    {   // 先拿出条件判断需要的变量,并将中间语句写入原Basicblock
        std::string str1 = cond->translate();
        // 新开一个trueBlock
        trueBlock = searchBlock(target);
        std::string str2 = "if " + str1 + " goto Block";
        str2 += std::to_string(trueBlock.num());
        // 新开一个falseBlock
        falseBlock = searchBlock(line+1);
        std::string str3 = "if False " + str1 + " goto Block";
        str3 += std::to_string(falseBlock.num());
        // 原Basicblock里面添上条件跳转的3AC作为结尾
        CFG[CurBlock].add(str2);
        CFG[CurBlock].add(str3);
        // 如果需要一个新的Basicblock, 将其入队
        if(falseBlock.empty())    Flow.push(falseBlock);
        if(trueBlock.empty())     Flow.push(trueBlock);
    }
};

class ForStmt: public Statement{
	Expression *expr;
public:	// 循环起头的行号在Statement类里面, 用CurFor将结尾的行号弄进来
	int end;
    Statement *stmt;
	ForStmt(int LINE, Statement *STMT, Expression *EXPR)
		:stmt(STMT), expr(EXPR) { line = LINE; type = stmt_for;}
	virtual void debug()
	{
		std::cout<<"FOR"<<"  (  ";
		stmt->debug();
		expr->debug();
		std::cout<<"  )"<<std::endl;
	}
    virtual void translate();
};

// Provide a simple FOR Statement stack
std::stack<ForStmt*> CurFor;
void ForStmt::translate()
{   // 遇到FOR语句, 直接新开一个Basicblock并将其写完
    // 在原Basicblock结尾先开一个跳转到新开的ctrlBlock
    std::string str1 = "goto Block";
    Basicblock ctrlBlock(searchBlock(line));
    str1 += std::to_string(ctrlBlock.num());
    CFG[CurBlock].add(str1);
    // 在ctrlBlock写入条件跳转语句的3AC
    CurBlock = ctrlBlock.num() - 1;
    std::string str2;
    str2 = expr->translate();
    // 新开一个trueBlock
    std::string str3 = "if " + str2 + " goto Block";
    trueBlock  = searchBlock(line+1);
    str3 += std::to_string(trueBlock.num());
    // 新开一个falseBlock
    std::string str4 = "if False " + str2 + " goto Block";
    falseBlock = searchBlock(end+1);
    str4 += std::to_string(falseBlock.num());
    // 在ctrlBlock写入条件跳转语句3AC作为ctrlBlock结尾
    CFG[CurBlock].add(str3);
    CFG[CurBlock].add(str4);
    CurFor.push(this);
    // 如果需要一个新的Block,将其入队
    if(falseBlock.empty())    Flow.push(falseBlock);
    if(trueBlock.empty())     Flow.push(trueBlock);
}

class EndStmt: public Statement{
	int target;
public:
	EndStmt(int LINE, int ENDFOR): 
		target(ENDFOR) { line = LINE; type = stmt_end; }
	virtual void debug()
	{
		std::cout<<"END"<<"  ";
		std::cout<<target<<std::endl;
	}
    virtual void translate()
    {   // END语句, 首先给自己FOR的trueBlock添加代表赋值的3AC
        VarStmt* tmp = reinterpret_cast<VarStmt*> (CurFor.top()->stmt);
        tmp->translate(); 
        // 找出自己的FOR的ctrlBlock
        Basicblock tmpBlock(searchBlock(target));
        // 在自己FOR的trueBlock后面加上goto语句作为结尾
        std::string res = "goto Block";
        res += std::to_string(tmpBlock.num());
        CFG[CurBlock].add(res);
        // ctrlBlock已经弄好了,所以不需要入队了
    }
};

class ExitStmt: public Statement{
	Expression *expr;
public:
	ExitStmt(int LINE, Expression *EXPR): 
		expr(EXPR) { line = LINE; type = stmt_exit; }
	virtual void debug()
	{
		std::cout<<"EXIT"<<"  ";
		expr->debug();
		std::cout<<std::endl;
	}
    virtual void translate()
    {
        std::string res = "program exit with ra = ";
        res += expr->translate();
        CFG[CurBlock].TAC.push_back(res);
    }
};
}


// --------- Parser ---------- //

// Provide a simple token buffer.
Token CurTok; // current token buffer
Token getNextToken() { return CurTok = Lexer.getNextToken(); } // 这个gettok考虑改一下


static std::map<Operator, int> BinopPrecedence; // a binop precedence table
int getTokPrecedence() { 			// function to get operand's precedence
  	if(CurTok.type != tok_op)	return -1;
  	int TokPrec = BinopPrecedence[CurTok.op];
  	return TokPrec;
}


// parsing Expressions
// parse the entire Expr and locate CurTok to the next
Expression *parseExpr(); 
// NumExpr -> numbers
Expression *parseNum()
{
	NumValue *res = new NumValue(CurTok.value);
	getNextToken();
	return res;
}
// ParenExpr -> '(' expression ')'
Expression *parseParen() 
{
 	getNextToken();
  	auto res = parseExpr();
  	getNextToken();
  	return res;
}
// IdentifierExpr -> Identifiers
Expression *parseIdentifier()
{
	Identifier *id = new Identifier(CurTok.name, CurTok.value);
	getNextToken();
	return id;
}
// primary 
//  -> identifierexpr
//  -> numberexpr
//  -> parenexpr
Expression *parsePrimary() {
  	switch (CurTok.type) {
  	case tok_identifier:
    	return parseIdentifier();
  	case tok_number:
    	return parseNum();
  	case tok_lparend:
    	return parseParen();
	default:
		return nullptr;
  	}
}
// BinOpRHS parses the sequence of pairs [binop, primaryExpr] for us
Expression *parseBinOpRHS(int ExprPrec, Expression *LHS) 
{
  	while (true) {
    	int TokPrec = getTokPrecedence();
    	// it's a sieve for such stream [binop, primary]...
    	if (TokPrec < ExprPrec)	return LHS;
        // eat binop 
    	Operator BinOp = CurTok.op;
    	getNextToken(); 
        // Parse the primary expression after the binary operator.
    	auto RHS = parsePrimary(); 
    	if (!RHS)  return nullptr; 
		// whether tight to pending 
    	int NextPrec = getTokPrecedence();
    	if (TokPrec < NextPrec) {		  
      		RHS = parseBinOpRHS(TokPrec + 1, RHS);
      		if(!RHS)	return nullptr;	   
    	}
    	// Merge LHS/RHS.
    	LHS = new BinExpr(BinOp, LHS, RHS);
  	}
}
// expression -> primary binoprhs
Expression *parseExpr() {
  	auto LHS = parsePrimary();
  	if (!LHS)  return nullptr;
  	return parseBinOpRHS(0, LHS);
}


// VarStmt -> Identifier = Expression
Statement *parseVarStmt()
{
	Identifier *id = new Identifier(CurTok.name);
	int line = CurTok.line;
	getNextToken(); 	// eat LHS, thus the variety's identifier, then CurTok is '='
	getNextToken(); 	// eat '=', thus ready to process RHS
	if(CurTok.type == tok_op && CurTok.op == '-') {
		getNextToken();	// if expression is "var = -1", then eat '-'
		auto *expr = new NumValue(- CurTok.value);
		auto *res  = new VarStmt(line ,id, expr);
		return res;
	}
	auto *expr = parseExpr();
	auto *res  = new VarStmt(line, id, expr);
	return res;
}
// InputStmt -> Identifier
Statement *parseInputStmt()
{
	std::vector<Identifier*> id; // CurTok points at identifier
	Identifier *id1 = new Identifier(CurTok.name);
	int line = CurTok.line;
	id.push_back(id1);
	getNextToken(); 			// CurTok is either komma, or line_breaker
	while(CurTok.type == tok_komm) {
		getNextToken(); 		// CurTok points at identifier
		id1 = new Identifier(CurTok.name);
		id.push_back(id1);
		getNextToken();			// CurTok is either komma, or line_breaker
	}
	InputStmt* res = new InputStmt(line, id);
	return res;
}
// GotoStmt is a terminal
Statement *parseGotoStmt()
{
	int target = CurTok.value;
	int line = CurTok.line;
	getNextToken();
	GotoStmt *res = new GotoStmt(line, target);
	return res;
}
// IfStmt -> cond
Statement *parseIfStmt()
{
	auto expr = parseExpr(); 	
	int line = CurTok.line;	 	// CurTok is at "THEN"
	getNextToken(); 	 	 	// eat THEN
	int target = CurTok.value;  // CurTok is a num equals to target line number
	getNextToken();
	IfStmt *res = new IfStmt(line, expr, target);
	return res;
}
// ForStmt -> VarStmt and an Expr
Statement *parseForStmt()
{
	auto var = parseVarStmt();  // parseVarStmt -> id = expr
	int line = CurTok.line;		// CurTok is at ';'
	getNextToken(); 		    // eat ';'
	auto condexpr = parseExpr();
	ForStmt *res = new ForStmt(line, var, condexpr);
	CurFor.push(res);
	return res;
}
// EndStmt -> modify current for
Statement *parseEndStmt()
{
	int line = CurTok.line;	 // ENDFOR's line number
	CurFor.top()->end = line;// set last FOR's end line
	auto *res = new EndStmt(line, CurFor.top()->line);
	CurFor.pop();
	getNextToken();  		 // eat "FOR" of "END FOR"
	return  res;
}
// ExitStmt -> expr
Statement *parseExitStmt() 
{
	int line = CurTok.line;
	auto *expr = parseExpr();
	ExitStmt *res = new ExitStmt(line, expr);
	return res;
}

// Top-level Parsing
Statement *parseStmt()	  
{
	switch(CurTok.type) { // eat keywords
	case tok_let:	
		getNextToken();	  // eat "LET"
		//std::cout << "LET ";
		return parseVarStmt();
	case tok_input: 
		getNextToken();   // eat "INPUT"
		//std::cout << "INPUT ";
		return parseInputStmt();
	case tok_goto:	
		getNextToken();   // eat "GOTO"
		//std::cout << "GOTO ";
		return parseGotoStmt();
	case tok_if:	
		getNextToken();   // eat "IF"
		//std::cout << "IF ";
		return parseIfStmt();
	case tok_for:	
		getNextToken();   // eat "FOR"
		//std::cout << "FOR ";
		return parseForStmt();
	case tok_end:	
		getNextToken();   // eat "END"
		//std::cout << "END ";
		return parseEndStmt();
	case tok_exit:	
		getNextToken();	  // eat "EXIT"
		//std::cout << "EXIT ";
		return parseExitStmt();
	}
	return nullptr;
}

// Main Loop
void Parser()
{
	while(true) {
		if(CurTok.type == tok_eof)	
			break;
		else if(CurTok.type == tok_line_break)
			getNextToken();
		else 
			Program.push_back(parseStmt());
	}
}	
// debugger for Parser
void debug()
{
    putchar('\n');
	auto it = Program.begin();
	while(it != Program.end())
	{
		(*it)->debug();
		it++;
	}
}

// ----- Syntax Directed Translation ----- //
// Top-level Translating
// 注意开辟第一个Basicblock
void transBlock(Basicblock block)
{
    CurBlock = block.num() - 1;
    setIS(block.head);
    while(IS != Program.end() ) {
        switch((*IS)->type) {
        case Statement::stmt_var:
            (*IS)->translate(); break;
        case Statement::stmt_input:
            (*IS)->translate(); break;
        case Statement::stmt_goto:
            // 在当前Basicblock里面写完结尾,而且将GOTO的目标入队
            (*IS)->translate();         
            // 这意味着当前Basicblock已经结束,上层韩束自动定位到下一个Basicblock
            return;
        case Statement::stmt_if:
            // 在当前Basicblock里面写完结尾,而且将falseBlock和trueBlock均入队
            (*IS)->translate();
            // 这意味着当前Basicblock已经结束,上层函数自动定位到下一个Basicblock
            return;
        case Statement::stmt_for:
            // 在当前ctrlBlock里面写完结尾,而且将trueBlock和falseBlock均入队
            (*IS)->translate();
            // 这意味着当前ctrlBlock已经结束,上层函数自动定位到下一个Basicblock
            return;
        case Statement::stmt_end:
            // 在当前trueBlock里面写完结尾,不需要将谁入队
            (*IS)->translate();
            // 这意味着当前trueBlock已经结束,上层函数自动定位到下一个Basicblock
            return;
        case Statement::stmt_exit:
            // 在当前Basicblock里面写完结尾,不需要将谁入队
            (*IS)->translate();
            // 这意味着当前Basicblock已经结束,上层函数自动定位到下一个Basicblock
            return;
        };
        ++IS;
    }
}
// Main loop
void Translate()
{
    // initialize
    IS = Program.begin();
    CurBlock = TotBlock = 0;
    Basicblock firstBlock((*IS)->line);
    CFG.push_back(firstBlock);
    Flow.push(firstBlock);
    // Flow里面有一个firstBlock了
    while(!Flow.empty() ) {
        if(!CtrlTable[Flow.front().head]) {
            transBlock(Flow.front());
            CtrlTable[Flow.front().head] = 1;
        }
        Flow.pop();
    }    
}


// main driver code

int main() {
  	// 1 is lowest precedence.
	BinopPrecedence[ASSIGN] = 1;
	BinopPrecedence[OR]    = 10;
	BinopPrecedence[AND]   = 20;
	BinopPrecedence[EQL]   = 30;
	BinopPrecedence[NEQ]   = 30;
	BinopPrecedence[LESS]  = 40;
	BinopPrecedence[GRT]   = 40;
	BinopPrecedence[LE]    = 40;
	BinopPrecedence[GE]    = 40;
  	BinopPrecedence[ADD]   = 50;
  	BinopPrecedence[MINUS] = 50;
  	BinopPrecedence[PROD]  = 60;
	BinopPrecedence[DIV]   = 60;

  	// Prime the first token.
	std::cin >> Curline;
	Lexer.lexer();
	getNextToken();

	// parse the code
	Parser();
	//debug();

    // translate 3AC 
    Translate();
    auto it = CFG.begin();
    while(it != CFG.end())
        it->demo(), it++;

  	return 0;
}
