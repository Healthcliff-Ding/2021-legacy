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
    // identifier:[a-z,A-Z] and keywords:[A-Z]
    if (isalpha(LastChar)) {     
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
            // in Wndows OS, EOF can only occur at the head of next line
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
    // Number:[0-9], only inner class in BASIC is int
    if(isdigit(LastChar)) {   
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar));
        ungetc(LastChar, stdin); 

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
    // +-*/, && || !=, == < > <= >=, and ; = \n
    char ThisChar = LastChar;
    // one byte operator
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
    // two bytes operator
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
int CurTmp;     // current tmp variety number
int CurBlock;   // current Basicblock of 3AC, use index in CFG's
int TotBlock;   // total Basicblock number

class Basicblock {
    int L;      // Block number
    std::string tmp;
public:
    std::vector<std::string> TAC;
    int head;   // first line of Block
    Basicblock() {}
    Basicblock(int HEAD): head(HEAD) { L = ++TotBlock; }
    void add(std::string tac)
    { 
        tmp = tac;
        TAC.push_back(tmp); 
    }
    int num() { return L; }
    void demo()
    {
        std::cout << "Basicblock " << L << "\tLine " << head << std::endl;
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
// find the Basicblock with same head number, or create a new Basicblock with that number
Basicblock searchBlock(int LINE) 
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
public:
	std::string name;
	Identifier(std::string NAME): 
		name(NAME) { type = expr_id; }
	virtual void debug()
	{
		std::cout << '(' << name << ')';
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
        std::string rd("t");
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

std::list<Statement*> Program;      // a dummy AST root, thus the Start of the language
std::list<Statement*>::iterator IS; // Istruction Selector, selects BASIC instruction
void setIS(int LINE)                // move IS to target line in BASIC
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
    virtual void translate();
};

class ForStmt: public Statement{
	Expression *expr;
public:	
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
{   // create and finish the ctrlBlock of current FOR
    // finish current Basicblock
    std::string str1 = "goto Block";
    Basicblock ctrlBlock(searchBlock(line));
    str1 += std::to_string(ctrlBlock.num());
    CFG[CurBlock].add(str1);
    // write condition branch 3AC into ctrlBlock
    CurBlock = ctrlBlock.num() - 1;
    std::string str2;
    str2 = expr->translate();
    // create the trueBlock
    std::string str3 = "if " + str2 + " goto Block";
    std::list<Statement*>::iterator tmpIter = IS; ++IS;
    trueBlock  = searchBlock((*IS)->line);
    str3 += std::to_string(trueBlock.num());
    // create the falseBlock
    std::string str4 = "if False " + str2 + " goto Block";
    setIS(end); ++IS;
    falseBlock = searchBlock((*IS)->line);
    str4 += std::to_string(falseBlock.num());
    // restore IS
    IS = tmpIter;
    // finish current Basicblock, thus ctrlBlock
    CFG[CurBlock].add(str3);
    CFG[CurBlock].add(str4);
    CurFor.push(this);
    // create the endBlock
    Basicblock endBlock = searchBlock(end);
    // enqueue to convey BFS of CFG
    Flow.push(falseBlock);
    Flow.push(trueBlock);
    Flow.push(endBlock);
}

void IfStmt::translate()
{   
    std::string str1 = cond->translate();
    // create the trueBlock
    trueBlock = searchBlock(target);
    if(!CurFor.empty() && target == (CurFor.top())->line)
        trueBlock = searchBlock((CurFor.top())->end);
    std::string str2 = "if " + str1 + " goto Block";
    str2 += std::to_string(trueBlock.num());
    // create the falseBlock
    ++IS;
    falseBlock = searchBlock((*IS)->line); --IS;
    std::string str3 = "if False " + str1 + " goto Block";
    str3 += std::to_string(falseBlock.num());
    // finish current Basicblock
    CFG[CurBlock].add(str2);
    CFG[CurBlock].add(str3);
    // enqueue to convey BFS of CFG
    Flow.push(falseBlock);
    Flow.push(trueBlock);
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
    {   // write variety convey 3AC into endBlock
        VarStmt* tmp = reinterpret_cast<VarStmt*> (CurFor.top()->stmt);
        CurFor.pop();
        tmp->translate(); 
        // find corresponding ctrlBlock
        Basicblock ctrlBlock = searchBlock(target);
        // finish endBlock
        std::string str2 = "goto Block";
        str2 += std::to_string(ctrlBlock.num());
        CFG[CurBlock].add(str2);
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
    {   // find/create target Basicblock
        Basicblock tmpBlock = searchBlock(target);
        // process continue
        // eitehr target is corresponding FOR's head or end
        // jump to its endBlock
        if(!CurFor.empty() && target == (CurFor.top())->line) 
            tmpBlock = searchBlock((CurFor.top())->end);
        // finish current Basicblock
        std::string res = "goto Block";
        res += std::to_string(tmpBlock.num());
        CFG[CurBlock].add(res);
        // enqueue to convey BFS of CFG
        Flow.push(tmpBlock);
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
Token getNextToken() 
{ 
    return CurTok = Lexer.getNextToken(); 
}


static std::map<Operator, int> BinopPrecedence; // a binop precedence table
int getTokPrecedence()              // function to get operand's precedence
{ 			
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
	Identifier *id = new Identifier(CurTok.name);
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
		// whether tight to pending 
    	int NextPrec = getTokPrecedence();
    	if (TokPrec < NextPrec)	  
      		RHS = parseBinOpRHS(TokPrec + 1, RHS); 
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
	if(CurTok.type == tok_op && CurTok.op == MINUS) {
		getNextToken();	// if expression is "var = -1", then eat '-'
		auto *expr = new NumValue(- CurTok.value);
		auto *res  = new VarStmt(line ,id, expr);
		getNextToken();
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
		return parseVarStmt();
	case tok_input: 
		getNextToken();   // eat "INPUT"
		return parseInputStmt();
	case tok_goto:	
		getNextToken();   // eat "GOTO"
		return parseGotoStmt();
	case tok_if:	
		getNextToken();   // eat "IF"
		return parseIfStmt();
	case tok_for:	
		getNextToken();   // eat "FOR"
		return parseForStmt();
	case tok_end:	
		getNextToken();   // eat "END"
		return parseEndStmt();
	case tok_exit:	
		getNextToken();	  // eat "EXIT"
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
bool isend;
void transBlock(Basicblock block)
{
    CurBlock = block.num() - 1;
    setIS(block.head);
    while(IS != Program.end() ) {
        switch((*IS)->type) {
        case Statement::stmt_var:
            (*IS)->translate(); 
            CurTmp = 0; break;
        case Statement::stmt_input:
            (*IS)->translate();
            CurTmp = 0; break;
        case Statement::stmt_goto:
            (*IS)->translate();  
            // relocate Basicblock       
            CurTmp = 0; return;
        case Statement::stmt_if:
            (*IS)->translate();
            // relocate Basicblock
            CurTmp = 0; return;
        case Statement::stmt_for:
            (*IS)->translate();
            // relocate Basicblock
            CurTmp = 0; return;
        case Statement::stmt_end:
            // just finish other Basicblock
            if(!isend)
            {
                int tmp = searchBlock((*IS)->line).num();
                std::string str1 = "goto Block ";
                str1 += std::to_string(tmp);
                CFG[CurBlock].add(str1);
                return ;
            }
            // write vareity convey 3AC into endBlock
            else {
                (*IS)->translate(); 
                CurTmp = 0; return;
            }
        case Statement::stmt_exit:
            // relocate Basicblock
            (*IS)->translate();
            CurTmp = 0; return;
        };
        ++IS;
        isend = false;
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

    while(!Flow.empty() ) {
        if(!CtrlTable[Flow.front().head]) { // std::map returns a false as no such key inside
            isend = true;
            transBlock(Flow.front());
            CtrlTable[Flow.front().head] = true;

        }
        Flow.pop();
    }    
}

// ----- Register to Memory ----- //
std::map<std::string, int> SymbolTable;
void Reg2Mem() {} // store varieties in register to memory


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

	// Parse the code
	Parser();

    // Translate to 3AC 
    Translate();
    auto it = CFG.begin();
    while(it != CFG.end()) {
        it->demo();
        ++it;
    }

  	return 0;
}
