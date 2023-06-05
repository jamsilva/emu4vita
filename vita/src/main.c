#include <psp2/kernel/processmgr.h>

#include "init.h"

int main(int argc, char *const argv[])
{
    AppInit(argc, argv);
    AppDeinit();

    sceKernelExitProcess(0);
    return 0;
}
