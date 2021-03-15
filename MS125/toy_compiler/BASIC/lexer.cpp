#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

// 这就是先写一部分, 为了debug写一个main弄成.cpp

// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.
// TokenType
enum tok_type {
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
enum Operator{
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


// gettok - Return the next token from standard input.
Token gettok() {
    char LastChar = ' ';

    // Skip any whitespace.
    while (isblank(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) {    // identifier:[a-z,A-Z] and keywords:[A-Z]
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

// lexer - return tokens in a line
// 或者靠这个lexer直接把所有的输入都解决了
// 问题又来了,我们的GOTO指令要求我们记录行号
// 下一个问题, 圆括号没有处理
// 也就是说圆括号只是需要一个接口
std::vector<Token> lexer()
{
    Token LastTok;
    std::vector<Token> text;
    do {
        LastTok = gettok();
        text.push_back(LastTok);
    }
    while(LastTok.type != tok_eof);

    return text;
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

// 现在需要测试这个lexer
// 先想好这个lexer的接口长什么样

// 另一个很重要的问题是, 还得想好行数的问题
// 存一个line_num 或者存line_break

int main()
{
    std::cin >> Curline;
    std::vector<Token>Lexer = lexer();
    for(int i=0; i<Lexer.size(); ++i)   std::cout << Lexer[i];
    putchar('\n');
}