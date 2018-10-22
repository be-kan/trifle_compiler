#include <vector>
#include "trifle.h"

using namespace std;

#define FirstAddr 2

Token NextToken;

int CurNameIndex = 0;
int CurFuncIndex;
int Level = -1;
int LastIndex[MAX_LEVEL];
int LastVarAddr[MAX_LEVEL];
int LasrVarAddr;

vector <NameItem> NameTable(MAX_TABLE_ENTRY);

void compileBlock(int indx);
void expression();

void initBlock(int firstAddr)
{
    if (Level == -1) {
        LasrVarAddr = firstAddr;
        CurNameIndex = 0;
        Level += 1;
        return;
    }
    LastIndex[Level] = CurNameIndex;
    LastVarAddr[Level] = LasrVarAddr;
    LasrVarAddr = firstAddr;
    Level += 1;
    return;
}

void finBlock()
{
    Level--;
    CurNameIndex = LastIndex[Level];
    LasrVarAddr = LastVarAddr[Level];
}

int getCurLevel()
{
    return Level;
}

int getFuncParams()
{
    if (Level < 1) return 0;
    return NameTable[LastIndex[Level-1]].val.params;
}

int regProgName(string id, int v)
{
    if (CurNameIndex++ < MAX_TABLE_ENTRY) NameTable[CurNameIndex].name = id;
    NameTable[CurNameIndex].val.type = progId;
    NameTable[CurNameIndex].val.param1 = Level;
    NameTable[CurNameIndex].val.param2 = v;
    return CurNameIndex;
}

int regFuncName(string id, int v)
{
    if (CurNameIndex++ < MAX_TABLE_ENTRY) NameTable[CurNameIndex].name = id;
    NameTable[CurNameIndex].val.type = funcId;
    NameTable[CurNameIndex].val.param1 = Level;
    NameTable[CurNameIndex].val.param2 = v;
    NameTable[CurNameIndex].val.params = 0;
    CurFuncIndex = CurNameIndex;
    return CurNameIndex;
}

int regFuncParam(string id)
{
    if (CurNameIndex++ < MAX_TABLE_ENTRY) NameTable[CurNameIndex].name = id;
    NameTable[CurNameIndex].val.type = parId;
    NameTable[CurNameIndex].val.param1 = Level;
    NameTable[CurFuncIndex].val.params++;
    return CurNameIndex;
}

int regVarName(string id)
{
    if (CurNameIndex++ < MAX_TABLE_ENTRY) NameTable[CurNameIndex].name = id;
    NameTable[CurNameIndex].val.type = varId;
    NameTable[CurNameIndex].val.param1 = Level;
    NameTable[CurNameIndex].val.param2 = LasrVarAddr++;
    return CurNameIndex;
}

int regConst(string id, int v)
{
    if (CurNameIndex++ < MAX_TABLE_ENTRY) NameTable[CurNameIndex].name = id;
    NameTable[CurNameIndex].val.type = constId;
    NameTable[CurNameIndex].val.param1 = v;
    return CurNameIndex;
}

void finParamDecl()
{
    int prs = NameTable[CurFuncIndex].val.params;
    if (prs == 0) return;
    for (int i=1; i<=prs; i++) NameTable[CurFuncIndex+i].val.param2 = i-1-prs;
}

void changeV(int indx, int newVal)
{
    NameTable[indx].val.param2 = newVal;
}

int searchId(string id, TypeOfId k)
{
    int i = CurNameIndex;
    NameTable[0].name = id;
    while (id.compare(string(NameTable[i].name))) i--;
    if (i) return i;
    else {
        if (k == varId) return regVarName(id);
        return 0;
    }
}

TypeOfId getIDType(int indx)
{
    return NameTable[indx].val.type;
}

int getAddress(int indx)
{
    return NameTable[indx].val.param2;
}

int getLevel(int indx)
{
    return NameTable[indx].val.param1;
}

int getVal(int indx)
{
    return NameTable[indx].val.param1;
}

int getLastVarAddr()
{
    return LasrVarAddr;
}

void compile()
{
    init();
    NextToken = nextToken();
    initBlock(FirstAddr);
    compileBlock(0);
    listCode();
    outputObjectCode();
}

void compileConstDecl()
{
    Token temp;
    while (1) {
        temp = NextToken;
        NextToken = checkAndgetToken(nextToken(), Equal);
        regConst(temp.name, NextToken.value);
        NextToken = nextToken();

        if (NextToken.type != Comma) {
            if (NextToken.type == Id) continue;
            else break;
        }
        NextToken = nextToken();
    }
    NextToken = checkAndgetToken(NextToken, Semicolon);
}

void compileVarDecl()
{
    int count = 0;
    while (1) {
        if (++count > MAX_LOOP) return;
        regVarName(NextToken.name);
        NextToken = nextToken();
        if (NextToken.type != Comma) {
            if (NextToken.type == Id) continue;
            else break;
        }
        NextToken = nextToken();
    }
    NextToken = checkAndgetToken(NextToken, Semicolon);
}

void compileFuncDecl()
{
    int fIndex;
    fIndex = regFuncName(NextToken.name, getNextCodeAddrs());
    NextToken = checkAndgetToken(nextToken(), Lparen);
    initBlock(FirstAddr);
    while (1) {
        if (NextToken.type == Id) {
            regFuncParam(NextToken.name);
            NextToken = nextToken();
        } else break;
        if (NextToken.type != Comma) {
            if (NextToken.type == Id) continue;
            else break;
        }
        NextToken = nextToken();
    }
    NextToken = checkAndgetToken(NextToken, Rparen);
    finParamDecl();
    if (NextToken.type == Semicolon) NextToken = nextToken();
    compileBlock(fIndex);
    NextToken = checkAndgetToken(NextToken, Semicolon);
}

void compileProgDecl()
{
    int fIndex;
    fIndex = regProgName(NextToken.name, getNextCodeAddrs());
    NextToken = nextToken();
    compileBlock(0);
}

static bool isStBeginKey(Token t)
{
    switch (t.type) {
        case If:
        case LBrace:
        case Call:
        case Ret:
        case While:
        case Write:
        case WriteLn:
        case WriteSp:
            return true;
        default:
            return false;
    }
}

void factor()
{
    int indx, i;
    TypeOfId kk;
    if (NextToken.type == Id) {
        indx = searchId(NextToken.name, varId);
        kk = getIDType(indx);
        switch (kk) {
            case varId:
            case parId:
                genObjCode(load, indx);
                NextToken = nextToken();
                break;
            case constId:
                genObjCode(literl, getVal(indx));
                NextToken = nextToken();
                break;
            case funcId:
                NextToken = nextToken();
                if (NextToken.type != Rparen) {
                    while (1) {
                        expression();
                        i++;
                        if (NextToken.type == Comma) {
                            NextToken = nextToken();
                            continue;
                        }
                        NextToken = checkAndgetToken(NextToken, Rparen);
                        break;
                    }
                } else NextToken = nextToken();
                genObjCode(call, indx);
                break;
            default:
                break;
        }
    } else if (NextToken.type == Num) {
        genObjCode(literl, NextToken.value);
        NextToken = nextToken();
    } else if (NextToken.type == Lparen) {
        NextToken = nextToken();
        expression();
        NextToken = checkAndgetToken(NextToken, Rparen);
    }

    switch (NextToken.type) {
        case Id:
        case Num:
        case Lparen:
            factor();
        default:
            return;
    }
}

void term()
{
    factor();
    TokType id = NextToken.type;
    while (id==Mult || id==Div) {
        NextToken = nextToken();
        factor();
        if (id==Mult) genObjCode(mul);
        else genObjCode(divide);
        id = NextToken.type;
    }
}

void expression()
{
    TokType id = NextToken.type;
    if (id==Plus || id==Minus) {
        NextToken = nextToken();
        term();
        if (id==Minus) genObjCode(neg);
    } else term();

    id = NextToken.type;
    while (id==Plus || id==Minus){
        NextToken = nextToken();
        term();
        if (id == Minus) genObjCode(sub);
        else genObjCode(add);
        id = NextToken.type;
    }
}

void condition()
{
    expression();
    TokType id = NextToken.type;
    switch (id) {
        case Equal:
        case Lss:
        case Gtr:
        case NotEq:
        case LssEq:
        case GtrEq:
            break;
        default:
            break;
    }
    NextToken = nextToken();
    expression();
    switch (id) {
        case Equal:
            genObjCode(equals);
            break;
        case Lss:
            genObjCode(ls);
            break;
        case Gtr:
            genObjCode(gr);
            break;
        case NotEq:
            genObjCode(neq);
            break;
        case LssEq:
            genObjCode(lseq);
            break;
        case GtrEq:
            genObjCode(greq);
            break;
        default:
            break;
    }
}

void statement()
{
    int indx;
    TypeOfId k;
    int patchAddress, patchAddress1;

    while (1) {
        switch(NextToken.type) {
            case Id:
                indx = searchId(NextToken.name, varId);
                k = getIDType(indx);
                NextToken = checkAndgetToken(nextToken(), Assign);
                expression();
                genObjCode(store, indx);
                return;
            case If:
                NextToken = nextToken();
                condition();
                NextToken = checkAndgetToken(NextToken, Then);
                patchAddress = genObjCode(jpc, 0);
                statement();
                backPatch(patchAddress);
                return;
            case Ret:
                NextToken = nextToken();
                expression();
                genRetCode();
                return;
            case LBrace:
                NextToken = nextToken();
                while (1) {
                    statement();
                    if (NextToken.type == Semicolon) {
                        NextToken = nextToken();
                        continue;
                    }
                    if (NextToken.type == RBrace) {
                        NextToken = nextToken();
                        return;
                    }
                    if (isStBeginKey(NextToken)) {
                        break;
                    }
                    NextToken = nextToken();
                }
            case While:
                NextToken = nextToken();
                patchAddress1 = getNextCodeAddrs();
                condition();
                NextToken = checkAndgetToken(NextToken, Do);
                patchAddress = genObjCode(jpc, 0);
                statement();
                genObjCode(jmp, patchAddress1);
                backPatch(patchAddress);
                return;
            case Call:
                NextToken = nextToken();
                expression();
                return;
            case Write:
                NextToken = nextToken();
                expression();
                genObjCode(wrt);
                return;
            case WriteLn:
                NextToken = nextToken();
                genObjCode(wrtln);
                return;
            case WriteSp:
                NextToken = nextToken();
                genObjCode(wrtsp);
                return;
            case RBrace:
            case Semicolon:
                return;
            default:
                NextToken = nextToken();
                continue;
        }
    }
}

void compileBlock(int indx)
{
    int patchAddress = genObjCode(jmp, 0);
    while (1) {
        switch(NextToken.type) {
            case Const:
                NextToken = nextToken();
                compileConstDecl();
                continue;
            case Integer:
                NextToken = nextToken();
                compileVarDecl();
                continue;
            case Func:
                NextToken = nextToken();
                compileFuncDecl();
                continue;
            case Prog:
                backPatch(0);
                NextToken = nextToken();
                compileProgDecl();
                return;
            default:
                break;
        }
        break;
    }
    backPatch(patchAddress);
    changeV(indx, getNextCodeAddrs());
    genObjCode(incmnt, getLastVarAddr());
    statement();
    genRetCode();
    finBlock();
}