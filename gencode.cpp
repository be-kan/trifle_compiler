#include "trifle.h"
#include <vector>

string ObjFileName;

vector <Instruction> Codes;

int getNextCodeAddrs()
{
    return Codes.size();
}

int genObjCode(OpCode op)
{
    Instruction inst = Instruction(op);
    Codes.push_back(inst);
    return Codes.size() - 1;
}

int genObjCode(OpCode op, int v)
{
    Instruction inst;
    if (op==load || op==call || op==store) inst = Instruction(op, getLevel(v), getAddress(v));
    else inst = Instruction(op, v);
    Codes.push_back(inst);
    return Codes.size() - 1;
}

int genRetCode()
{
    if (Codes[Codes.size()-1].opCode == ret) return Codes.size()-1;
    Instruction inst = Instruction(ret, getCurLevel(), getFuncParams());
    Codes.push_back(inst);
    return Codes.size()-1;
}

void backPatch(int indx)
{
    Codes[indx].param1 = Codes.size();
}

void listCode()
{
    cout << "----- generated code -----" << endl;
    int n = (int)Codes.size();
    for (int i=0; i < n; i++) {
        cout << setw(3) << i;
        cout << ": ";
        printCode(Codes[i]);
    }
}

void outputCode(ofstream &fout, Instruction instrct)
{
    int flag;
    fout << (char)(instrct.opCode & 0xff);
    switch (instrct.opCode) {
        case ret:
        case load:
        case call:
        case store:
            flag = 2;
            break;
        case literl:
        case incmnt:
        case jmp:
        case jpc:
            flag = 1;
            break;
        default:
            flag = 0;
            break;
    }

    switch (flag) {
        case 1:
            fout << setw(8) << instrct.param1;
            break;
        case 2:
            fout << (char)(instrct.param1 & 0xff);
            fout << (char)(instrct.param2 & 0xff);
            break;
        default:
            break;
    }
}

int outputObjectCode()
{
    ObjFileName = SrcFileName.replace(SrcFileName.find(".tri"), 4, ".exf");
    char fname[MAX_PATH_LEN];
    strcpy(fname, ObjFileName.c_str());
    ofstream fout((const char * )fname, ios::out | ios::binary);
    int n = (int)Codes.size();
    for (int i=0; i<n; i++) {
        outputCode(fout, Codes[i]);
    }
    fout.close();
    return 0;
}