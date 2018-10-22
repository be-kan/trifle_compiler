#ifndef CODEGEN_H
#define CODEGEN_H 1

using namespace std;

#define MAX_LOOP 20

typedef enum codes {
    literl, load, store, call, ret, incmnt, jmp, jpc,
    neg, add, sub, mul, divide,
    equals, ls, gr, neq, lseq, greq,
    wrt, wrtln, wrtsp,
    nop, e_nd, dummy
} OpCode;

class Instruction {
public:
    OpCode opCode;
    int param1;
    int param2;
    Instruction(OpCode opcode = nop, int p1 = 0, int p2 = 0) {
        opCode = opcode;
        param1 = p1;
        param2 = p2;
    }
};

int genObjCode(OpCode op, int v);
int genObjCode(OpCode op);
int genRetCode();
void backPatch(int indx);
int getNextCodeAddrs();
void listCode();
int outputObjectCode();

#endif