#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/power.h>
#include <psp2/apputil.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/system_param.h>
#include <psp2/appmgr.h>
#include <psp2/shellutil.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/devctl.h>
#include <psp2/vshbridge.h>
#include <psp2/common_dialog.h>

#include <vita2d.h>

#include "init.h"
#include "utils.h"
#include "file.h"
#include "ui.h"
#include "config.h"

int is_safe_mode = 0;
int is_vitatv_model = 0;

int language = 0, enter_button = 0, date_format = 0, time_format = 0;

int isSafeMode()
{
    return is_safe_mode;
}

int checkSafeMode()
{
    if (sceIoDevctl("ux0:", 0x3001, NULL, 0, NULL, 0) == 0x80010030)
        is_safe_mode = 1;

    return is_safe_mode;
}

int isVitatvModel()
{
    return is_vitatv_model;
}

int checkVitatvModel()
{
    if (sceKernelGetModel() == SCE_KERNEL_MODEL_VITATV)
        is_vitatv_model = 1;

    return is_vitatv_model;
}

static void initVita2dLib()
{
    vita2d_init();
    vita2d_set_vblank_wait(0);
}

static void finishVita2dLib()
{
    vita2d_fini();
}

static void initSceAppUtil()
{
    // Init SceAppUtil
    SceAppUtilInitParam init_param;
    SceAppUtilBootParam boot_param;
    memset(&init_param, 0, sizeof(SceAppUtilInitParam));
    memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
    sceAppUtilInit(&init_param, &boot_param);

    // System params
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &language);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, &enter_button);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_DATE_FORMAT, &date_format);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_TIME_FORMAT, &time_format);

    // Set common dialog config
    SceCommonDialogConfigParam config;
    sceCommonDialogConfigParamInit(&config);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, (int *)&config.language);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, (int *)&config.enterButtonAssign);
    sceCommonDialogSetConfigParam(&config);
}

static void finishSceAppUtil()
{
    // Shutdown AppUtil
    sceAppUtilShutdown();
}

int initMain()
{
    sceIoRemove(APP_LOG_PATH);
    createFolder(APP_DATA_DIR);

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceShellUtilInitEvents(0);
    initSceAppUtil();

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);

    checkVitatvModel();

    initVita2dLib();
    initUi();

    if (checkSafeMode())
    {
        printSafeMode();
    }

    lockUsbConnection();

    return 0;
}

int finishMain()
{
    finishUi();
    finishVita2dLib();
    finishSceAppUtil();

    return 0;
}
