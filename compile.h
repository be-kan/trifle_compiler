#ifndef COMPILE_H
#define COMPILE_H 1

#include <string>

#define MAX_TABLE_ENTRY 100
#define MAX_LEVEL 5

typedef enum IDTypes {
    varId, funcId, progId, parId, constId, IDNull
} TypeOfId;

class NameValues {
public:
    TypeOfId type;
    int value;
    int param2;
    int param1;
    int params;
};

class NameItem {
public:
    string name;
    NameValues val;
};

int getCurLevel();
int getFuncParams();
void changeV(int ti, int newVal);
int getAddress(int ti);
int getLevel(int ti);
void compile();

#endif
