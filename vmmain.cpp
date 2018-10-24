#include <vector>
#include "tvm.h"

using namespace std;

#define MEMSIZE 4096
#define MAX_LEVEL 5

void openFile(string );
void closeFile();
void loadBinData();

vector <Instruction> ObjCodes;
string ObjFileName;

ifstream fin;

class VirtualMachine
{
    vector <int> vmemory;
    int pc;
    int stacktop;
    int display[MAX_LEVEL];
public:
    VirtualMachine() {
        vmemory = vector <int> (MEMSIZE);
        stacktop = 0;
        pc = 0;
    }
    void storemem (int addr, int getVal) { vmemory[addr] = getVal; }
    int loadmem (int addr) { return vmemory[addr]; }
    void push (int getVal) { vmemory[stacktop++] = getVal; }
    int pop() { return vmemory[--stacktop]; }
    int peek() { return vmemory[stacktop]; }
    int deep_peek() { return vmemory[stacktop-1]; }
    int getStackTop() { return stacktop; }
    int setStackTop(int addr) {
        stacktop = addr;
        return stacktop;
    }
    int getPc() { return pc; }
    void setPc(int addr) { pc = addr; }
    int incPc() { return pc++; }
    int getDisplay(int level) { return display[level]; }
    void setDisplay(int level, int getVal) { display[level] = getVal; }
    void execute();
};

VirtualMachine vm;

int main (int argc, char *argv[])
{
    openFile(argv[1]);
    loadBinData();
    vm.execute();
    closeFile();

    return 0;
}

void openFile(string fileName)
{
    fin.open(fileName.c_str(), ios::binary | ios::in);
}

void closeFile()
{
    fin.close();
}

int getNextChar()
{
    int c = fin.get();
    if (c > 126) c = c - 256;
    return c;
}

int getValue()
{
    int v;
    fin >> setw(8) >> v;
    return v;
}

void loadBinData()
{
    Instruction instrct = Instruction();
    int c;
    while (true) {
        c = getNextChar();
        if (fin.eof()) break;
        instrct.opCode = (OpCode)c;
        switch (c) {
            case literl:
            case incmnt:
            case jmp:
            case jpc:
                instrct.param1 = getValue();
                break;
            case ret:
            case load:
            case call:
            case store:
                instrct.param1 = getNextChar();
                instrct.param2 = getNextChar();
                break;
            default:
                break;
        }
        ObjCodes.push_back(instrct);
    }
}

Instruction getNextCode()
{
    if (vm.getPc() >= (int)ObjCodes.size()) return Instruction(e_nd);
    return  ObjCodes[vm.incPc()];
}

void VirtualMachine::execute()
{
    int temp;
    Instruction instrct;

    storemem(0, 0);
    storemem(1, 0);
    display[0] = 0;

    do {
        instrct = getNextCode();

        switch(instrct.opCode){
            case literl:
                push(instrct.param1);
                break;
            case load:
                push(loadmem( getDisplay(instrct.param1) + instrct.param2) );
                break;
            case store:
                storemem( getDisplay(instrct.param1) + instrct.param2, pop() );
                break;
            case call:
                storemem(getStackTop(), display[instrct.param1+1]);
                storemem(getStackTop()+1, getPc());
                setDisplay(instrct.param1+1, getStackTop());
                setPc(instrct.param2) ;
                break;
            case ret:
                temp = pop();
                setStackTop(getDisplay(instrct.param1));
                if (instrct.param1 == 0) { setPc(0); break; }
                setDisplay(instrct.param1, peek());
                setPc(loadmem(getStackTop()+1));
                setStackTop(getStackTop() - instrct.param2);
                push(temp);
                break;
            case incmnt:
                setStackTop(getStackTop() + instrct.param1);
                break;
            case jmp:
                setPc(instrct.param1);
                break;
            case jpc:
                if (pop() == 0) setPc(instrct.param1);
                break;
            case neg:
                storemem( getStackTop()-1, -1 * loadmem(peek()-1) );
                break;
            case add:
                pop();
                storemem( getStackTop()-1, deep_peek() + peek() );
                break;
            case sub:
                pop();
                storemem( getStackTop()-1, deep_peek() - peek() );
                break;
            case mul:
                pop();
                storemem( getStackTop()-1, deep_peek() * peek() );
                break;
            case divide:
                pop();
                storemem( getStackTop()-1, deep_peek() / peek() );
                break;
            case equals:
                pop();
                storemem( getStackTop()-1, (deep_peek() == peek()) );
                break;
            case ls:
                pop();
                storemem( getStackTop()-1, (deep_peek() < peek()) );
                break;
            case gr:
                pop();
                storemem( getStackTop()-1, (deep_peek() > peek()) );
                break;
            case neq:
                pop();
                storemem( getStackTop()-1, (deep_peek() != peek()) );
                break;
            case lseq:
                pop();
                storemem( getStackTop()-1, (deep_peek() <= peek()) );
                break;
            case greq:
                pop();
                storemem( getStackTop()-1, (deep_peek() >= peek()) );
                break;
            case wrt:
                cout << pop();
                break;
            case wrtln:
                cout << endl;
                break;
            case wrtsp:
                cout << " ";
                break;
            default:
                break;
        }
    } while (getPc() != 0);
};