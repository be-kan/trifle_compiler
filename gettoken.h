
#ifndef GETSOURCE_H
#define GETSOURCE_H

using namespace std;

extern int CurLineNo;

#define MAXNAME 31

typedef enum types {
    LBrace, RBrace,
    If, Then,
    While, Do,
    Call, Ret, Func, Prog,
    Integer, Const,
    Write, WriteLn, WriteSp,
    end_of_KeyWd,
    Plus, Minus,
    Mult, Div,
    Lparen, Rparen,
    Equal, Lss, Gtr,
    NotEq, LssEq, GtrEq,
    Comma, Period, Semicolon,
    Assign,
    end_of_KeySym,
    Id, Num, Nul,
    end_of_Token,
    Letter, Digit, Colon, Others,
    EndOfFile
} TokType;

class Token {
public:
    TokType type;
    string name;
    int value;
    Token () { type=Others; name=""; value=0; }
};

void openSource(string );
void init();
void closeSource();
Token nextToken();
Token checkAndgetToken(Token t, TokType k);

#endif