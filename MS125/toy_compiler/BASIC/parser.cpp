#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <stack>

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
            case '&': return Token(tok_op, ADD);
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

std::ostream &operator<<(std::ostream &_os, Token _tok)
{
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
// 把Lexer改成一个类啊, 然而后来的代码并不用stdin
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
	// 继承的需要多写一个东西
	// 到时候的析构函数
	virtual ~Expression() = default;
};

class Identifier: public Expression{	
public:	// 弄上缺省值,预留以后的INPUT
	std::string name;
	int value;
	Identifier(std::string NAME, int VAL = 0): 
		name(NAME), value(VAL) { type = expr_id; }
};

class NumValue: public Expression{
public:
	int num;
	NumValue(int n): num(n) { type = expr_num; }
};

class BinExpr: public Expression{
public:
	Operator op;
	Expression *lhs, *rhs;
  	BinExpr(Operator OP, Expression *LHS, Expression *RHS)
		:op(OP), lhs(LHS), rhs(RHS) { type = expr_bin; }
};

std::ostream &operator<<(std::ostream &_os, Expression _expr){
	NumValue* num1;
	Identifier* id2;
	BinExpr* bin3;
	switch(_expr.type){
	case Expression::expr_num:	
		num1 = reinterpret_cast<NumValue*> (&_expr);
		return std::cout << ' ' << num1->num << ' ';	break;
	case Expression::expr_id:
		id2 = reinterpret_cast<Identifier*> (&_expr);
		return std::cout << ' ' << id2->name << ' ';	break;
	case Expression::expr_bin:
		bin3 = reinterpret_cast<BinExpr*> (&_expr);
		return std::cout << *(bin3->lhs) << bin3->op << *(bin3->rhs);	break;
	};
	return std::cout;
}

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
};

class VarStmt: public Statement{ // 变量声明
	Identifier *lhs; 			 // 因为做左值, 所以不用IdExpr
	Expression *rhs;
public:
	VarStmt(int LINE, Identifier *LHS, Expression *RHS): 
		lhs(LHS), rhs(RHS) { line = LINE; type = stmt_var;}
};

class InputStmt: public Statement{ // 有可能有一堆Identifier
	std::vector<Identifier*> id;
public:
	InputStmt(int LINE, std::vector<Identifier*> ID): 
		id(ID) { line = LINE; type = stmt_input; }
};

class GotoStmt: public Statement{
	int target;
public:
	GotoStmt(int LINE, int GOTO): 
		target(GOTO) { line = LINE; type = stmt_goto; }
};

class IfStmt: public Statement{
	Expression *cond;
	int target;
public:
	IfStmt(int LINE, Expression *EXPR, int THEN): 
		cond(EXPR), target(THEN) { line = LINE; type = stmt_if; }
};

class ForStmt: public Statement{
	Statement *stmt;
	Expression *expr;
public:	// 循环起头的行号在Statement类里面, 用CurFor将结尾的行号弄进来
	int end;
	ForStmt(int LINE, Statement *STMT, Expression *EXPR)
		:stmt(STMT), expr(EXPR) { line = LINE; type = stmt_for;}
};

class EndStmt: public Statement{
	int target;
public:
	EndStmt(int LINE, int ENDFOR): 
		target(ENDFOR) { line = LINE; type = stmt_end; }
};

class ExitStmt: public Statement{
	Expression *expr;
public:
	ExitStmt(int LINE, Expression *EXPR): 
		expr(EXPR) { line = LINE; type = stmt_exit; }
};

std::list<Statement*> Program;

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

// Provide a simple FOR Statement stack
std::stack<ForStmt*> CurFor;

// parsing Expressions
Expression *parseExpr(); // parse the entire Expr and locate CurTok to the next
// NumExpr -> numbers
Expression *parseNum()
{
	// get CurTok's attribute value
	NumValue *res = new NumValue(CurTok.value);
	getNextToken(); // consume the token, 这个token没了, CurTok里面应该是下一个Token了
	return res;
}
// ParenExpr -> '(' expression ')'
Expression *parseParen() 
{
 	getNextToken(); // eat (. 现在CurTok准备开始parseExpr
  	auto res = parseExpr();
	// 没有语法错误呢~~
  	getNextToken(); // eat ). CurTok里面是下一个Token了
  	return res;
}
// IdentifierExpr -> Identifiers
Expression *parseIdentifier()
{
	Identifier *id = new Identifier(CurTok.name, CurTok.value);
	getNextToken(); // consume the token, 这个token没了, CurTok里面应该是下一个Token了
	return id;
}
// primary 调用的三个函数会自动吃掉当前的Token,这样CurTok就是下一个Token了
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
    	// CurTok指向的是下一个Token, 而ExprPrec里面存的是结合LHS的运算符的优先级(即LHS左边运算符的优先级)
		// 如 a [* b] [+ c], 这里只能返回a-*-b的节点
		// 而由于getTokPrecedence设置的返回值有-1,所以起到了"筛"的作用
    	if (TokPrec < ExprPrec)	return LHS;

    	Operator BinOp = CurTok.op;
    	getNextToken(); // eat binop 那应该CurTok移到RHS, 准备parse RHS了

    	auto RHS = parsePrimary(); // Parse the primary expression after the binary operator.
    	if (!RHS)  return nullptr; // 没有语法错误 ~ ~
		// RHS要不要和后面的结合的问题
    	int NextPrec = getTokPrecedence(); // 这应该是作RHS的primaryExpr后面的的 binop
    	if (TokPrec < NextPrec) {		   // 然后发现RHS需要和RHS右边的下一个primaryExpr结合
      		RHS = parseBinOpRHS(TokPrec + 1, RHS);	// 赋值运算不能连等
      		if(!RHS)	return nullptr;	   // TokPrec是RHS左边的那个binop,而+1是为了左结合
    	}
    	// Merge LHS/RHS. 但是会不会内存泄漏
    	LHS = new BinExpr(BinOp, LHS, RHS);
  	}
}
// expression -> primary binoprhs
// 一个expr应该可以拆分成一个parse和一堆诸如[binop, primary]的流
// parseExpr只是parse掉这第一个primary,然后调用parseBinOpRHS(与LHS结合的binop的优先级, LHS)完成整个Expr的解析
// 但是问题来了, parseExpr到底是不是parse掉整个expr呢？
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
	GotoStmt *res = new GotoStmt(line, target);
	return res;
}
// IfStmt -> cond
Statement *parseIfStmt()
{
	auto expr = parseExpr(); 	// 默认parseExpr可以parse掉整个Expr
	int line = CurTok.line;	 	// CurTok is at "THEN"
	getNextToken(); 	 	 	// eat THEN
	int target = CurTok.value;  // CurTok is a num equals to target line number
	IfStmt *res = new IfStmt(line, expr, target);
	return res;
}
// ForStmt -> VarStmt 和一个 Expr
Statement *parseForStmt()
{
	auto var = parseVarStmt();  // parseVarStmt -> id = expr
	int line = CurTok.line;		// CurTok is at ';'
	getNextToken(); 		    // eat ';'
	auto condexpr = parseExpr();// 默认parseExpr可以处理整个表达式
	ForStmt *res = new ForStmt(line, var, condexpr);
	CurFor.push(res);
	return res;
}
// 为该要的for语句添上end
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
Statement *parseExitStmt() // EXIT必须后面跟着expr,没有EXIT才会返回0
{
	int line = CurTok.line;
	auto *expr = parseExpr();
	ExitStmt *res = new ExitStmt(line, expr);
	// 调试
	std::cout << *expr << std::endl;
	return res;
}

// Top-level Parsing
Statement *parseStmt()	  // parseStmt以后CurTok应指向line_breaker
{
	switch(CurTok.type) { // 在parseStmt先里弄掉标识符
	case tok_let:	
		getNextToken();	  // eat "LET"
		std::cout << "LET ";
		return parseVarStmt();
	case tok_input: 
		getNextToken();   // eat "INPUT"
		std::cout << "INPUT ";
		return parseInputStmt();
	case tok_goto:	
		getNextToken();   // eat "GOTO"
		std::cout << "GOTO ";
		return parseGotoStmt();
	case tok_if:	
		getNextToken();   // eat "IF"
		std::cout << "IF ";
		return parseIfStmt();
	case tok_for:	
		getNextToken();   // eat "FOR"
		std::cout << "FOR ";
		return parseForStmt();
	case tok_end:	
		getNextToken();   // eat "END"
		std::cout << "END ";
		return parseEndStmt();
	case tok_exit:	
		getNextToken();	  // eat "EXIT"
		std::cout << "EXIT ";
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
	// 一个方法是手动填充Lexer,text

	// parse the code
	Parser();
	// 目前发现parseExpr部分有问题

	// 采取直接 print 法
	// 应该让print显示调用的结果
	// 因此一条一条调用

  	return 0;
}
