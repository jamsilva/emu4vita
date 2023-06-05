#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2/kernel/processmgr.h>

#include "main.h"
#include "init.h"
#include "ui.h"
#include "boot.h"

static int vita_run = 1;

int loopMain()
{
    while (vita_run)
    {
        UiMain();
    }

    return 0;
}

int exitApp()
{
    vita_run = 0;

    return 0;
}

int main(int argc, char *const argv[])
{
    initMain();
    loopMain();
    finishMain();

    sceKernelExitProcess(0);
    return 0;
}
