
#include "trifle.h"

using namespace std;

char LineBuffer[MAX_LINE_LEN];
int CurLineNo;
int LineIndex;
char Chr;

Token CurToken;
Token PrevToken;

struct keyWd {
    string name;
    TokType keyId;
};

static struct keyWd KeyWdT[] = {
        {"{", LBrace}, {"}", RBrace},
        {"if", If}, {"then", Then},
        {"while", While}, {"do", Do},
        {"call", Call},{"return", Ret},
        {"function", Func}, {"program", Prog},
        {"int", Integer}, {"const", Const},
        {"write", Write}, {"writeln",WriteLn}, {"writesp",WriteSp},
        {"$dummy1",end_of_KeyWd},
        {"+", Plus}, {"-", Minus}, {"*", Mult}, {"/", Div},
        {"(", Lparen}, {")", Rparen},
        {"=", Equal}, {"<", Lss}, {">", Gtr},
        {"<>", NotEq}, {"<=", LssEq}, {">=", GtrEq},
        {",", Comma}, {".", Period}, {";", Semicolon},
        {":=", Assign},
        {"$dummy2",end_of_KeySym}
};

string keyname[] {
        "LBrace", "RBrace",
        "If", "Then",
        "While", "Do",
        "Call", "Return", "Function", "Program",
        "int", "Const",// "Odd",
        "Write", "WriteLn", "WriteSp",
        "end_of_KeyWd",
        "Plus", "Minus",
        "Mult", "Div",
        "Lparen", "Rparen",
        "Equal", "Lss", "Gtr",
        "NotEq", "LssEq", "GtrEq",
        "Comma", "Period", "Semicolon",
        "Assign",
        "end_of_KeySym",
        "Id", "Num", "Nul",
        "end_of_Token",
        "Letter", "Digit", "Colon", "Others",
        "EndOfFile"
};

ifstream Fin;

static TokType getCharType(char chr)
{
    if (chr >='0' && chr<='9') return Digit;
    if ((chr >='A' && chr<='Z') || (chr >='a' && chr<='z')) return Letter;

    switch (chr) {
        case '{': return LBrace;
        case '}': return RBrace;
        case '+': return Plus;
        case '-': return Minus;
        case '*': return Mult;
        case '/': return Div;
        case '(': return Lparen;
        case ')': return Rparen;
        case '=': return Equal;
        case '<': return Lss;
        case '>': return Gtr;
        case ',': return Comma;
        case '.': return Period;
        case ';': return Semicolon;
        case ':': return Colon;
        default: break;
    }

    return Others;
}

void init()
{
    LineIndex = -1;
    Chr = 0;
    CurToken = Token();
}

void openSource(string fileName)
{
    Fin.open(fileName.c_str(), ios::in); // ios::in means 'read'
    CurLineNo = 0;
}

void closeSource()
{
    Fin.close();
}

static char *removeComment(char *s)
{
    int len = strlen(s);
    char ws[MAX_LINE_LEN];
    char chr = 0;
    int j = 0;
    for (int i = 0; i < len; i++) {
        chr = s[i];
        if ((chr == '/') & (i < len-1)) {
            if (s[i+1] == '/') break;
        }
        ws[j++] = chr;
    }
    ws[j] = 0;
    strcpy(s, ws);

    return s;
}

static int nextChar()
{
    int ch;
    if (LineIndex == -1) {
        Fin.getline(LineBuffer, MAX_LINE_LEN);
        CurLineNo += 1;
        // cout << setw(3) << CurLineNo;
        // cout << ": " << LineBuffer << endl;
        LineIndex = 0;
        removeComment(LineBuffer);
    } else if (Fin.eof()) return EOF;

    ch = (int)LineBuffer[LineIndex++];
    if (ch == 0) LineIndex = -1;

    return ch;
}

Token nextToken()
{
    PrevToken = CurToken;
    TokType prvType = PrevToken.type;
    int i = 0;
    int num;
    Token temp;
    string id;

    while (1) {
        if (Chr==' ' || Chr==0x9 || Chr==0xa || Chr==0xd || Chr==0) ;
        else break;
        Chr = nextChar();
    }

    TokType cc = getCharType(Chr);
    switch (cc) {
        case LBrace:
        case RBrace:
            id.append(1, Chr);
            Chr = nextChar();
            for (i=0; i<end_of_KeyWd; i++) {
                if (id == KeyWdT[i].name) {
                    temp.type = KeyWdT[i].keyId;
                    CurToken = temp;
                    return temp;
                }
            }
            break;
        case Letter:
            do {
                if (i < MAXNAME) id.append(1, Chr);
                i++;
                Chr = nextChar();
            } while (getCharType(Chr)==Letter || getCharType(Chr)==Digit);

            if (i > MAXNAME) i = MAXNAME - 1;

            for (i=0; i<end_of_KeyWd; i++) {
                if (id == KeyWdT[i].name) {
                    temp.type = KeyWdT[i].keyId;
                    CurToken = temp;
                    return temp;
                }
            }

            temp.type = Id;
            temp.name = id;
            break;
        case Digit:
            num = 0;

            do {
                num = 10 * num + (Chr-'0');
                i++;
                Chr = nextChar();
            } while (getCharType(Chr)==Digit);

            temp.type = Num;
            temp.value = num;
            break;
        case Minus:
            if (prvType==Assign || prvType==Lparen || prvType==Comma || prvType==Plus ||
                prvType==Minus || prvType==Mult || prvType==Div) {
                Chr = nextChar();
                num = 0;

                do {
                    num = 10 * num + (Chr-'0');
                    i++;
                    Chr = nextChar();
                } while (getCharType(Chr)==Digit);

                temp.type = Num;
                temp.value = -1 * num;
            } else {
                temp.type = Minus;
                Chr = nextChar();
            }
            break;
        case Colon:
            if ((Chr = nextChar()) == '=') {
                Chr = nextChar();
                temp.type = Assign;
            } else temp.type = Nul;
            break;
        case Lss:
            if ((Chr = nextChar()) == '=') {
                Chr = nextChar();
                temp.type = LssEq;
            } else if (Chr == '>') {
                Chr = nextChar();
                temp.type = NotEq;
            } else temp.type = Lss;
            break;
        case Gtr:
            if ((Chr = nextChar()) == '=') {
                Chr = nextChar();
                temp.type = GtrEq;
            } else temp.type = Gtr;
            break;
        default:
            temp.type = cc;
            Chr = nextChar();
            break;
    }
    CurToken = temp;
    return temp;
}

Token checkAndgetToken(Token t, TokType k)
{
    if (t.type == k) {
        return nextToken();
    } else {
        return t;
    }
}