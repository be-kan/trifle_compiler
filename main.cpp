
#include "trifle.h"

using namespace std;

string SrcFileName;

int main(int argc, char *argv[])
{
    SrcFileName = string(argv[1]);
    openSource(SrcFileName);
    compile();
    closeSource();

    return 0;
}