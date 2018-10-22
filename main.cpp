
#include "trifle.h"

using namespace std;

#ifdef _MSC_VER
#include <process.h>
#define Exec _xeclp
#define TVM "tvm"
#else
#include <unistd.h>
#define Exec execlp
#define TVM "./tvm"
#endif

string SrcFileName;

int main(int argc, char *argv[])
{
    SrcFileName = string(argv[1]);
    openSource(SrcFileName);
    compile();
    closeSource();

    char fname[MAX_PATH_LEN];
    strcpy(fname, ObjFileName.c_str());
    cout << "----- execute output -----" << endl;
    Exec(TVM, TVM, fname, NULL);

    return 0;
}