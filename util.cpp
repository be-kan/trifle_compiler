#include <iostream>
#include "util.h"

using namespace std;

void printCode(Instruction instrct)
{
    int flag = 0;
    switch(instrct.opCode) {
        case literl: cout << "literl "; flag = 1; break;
        case neg:    cout << "neg ";              break;
        case add:    cout << "add ";              break;
        case sub:    cout << "sub ";              break;
        case mul:    cout << "mul ";              break;
        case divide: cout << "div ";              break;
        case equals: cout << "eq ";               break;
        case ls:     cout << "ls ";               break;
        case gr:     cout << "gr ";               break;
        case neq:    cout << "neq ";              break;
        case lseq:   cout << "lseq";              break;
        case greq:   cout << "greq";              break;
        case wrt:    cout << "wrt";               break;
        case wrtln:  cout << "wrtln";     flag=0; break;
        case wrtsp:  cout << "wrtsp";             break;
        case load:   cout << "load ";     flag=2; break;
        case store:  cout << "store ";    flag=2; break;
        case call:   cout << "call ";     flag=2; break;
        case ret:    cout << "ret ";      flag=2; break;
        case incmnt: cout << "incmnt ";   flag=1; break;
        case jmp:    cout << "jmp ";      flag=1; break;
        case jpc:    cout << "jpc ";      flag=1; break;
        default: break;
    }

    switch(flag){
        case 0:
            cout << endl;
            return;
        case 1:
            cout << instrct.param1 << endl;
            break;
        default:
            cout << instrct.param1 << ",";
            cout << instrct.param2 << endl;
            break;
    }
}