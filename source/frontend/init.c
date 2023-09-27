#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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
#include <psp2/touch.h>
#include <psp2/common_dialog.h>

#include "setting/setting.h"
#include "gui/gui.h"
#include "emu/emu.h"
#include "init.h"
#include "utils.h"
#include "file.h"
#include "config.h"
#include "boot.h"
#include "lang.h"

#ifdef SCE_LIBC_SIZE
unsigned int sceLibcHeapSize = SCE_LIBC_SIZE;
#endif

extern GUI_Activity browser_activity;

static int app_exit = 0;

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

static void safeModeDialogNegativeCallback(GUI_Dialog *dialog)
{
    AppExit();
}

static void showSafeModeDialog()
{
    GUI_Dialog *tip_dialog = AlertDialog_Create();
    AlertDialog_SetTitle(tip_dialog, cur_lang[TITLE_TIP]);
    AlertDialog_SetMessage(tip_dialog, cur_lang[MESSAGE_SAFE_MODE]);
    AlertDialog_SetNegativeButton(tip_dialog, cur_lang[EXIT], safeModeDialogNegativeCallback);
    AlertDialog_Show(tip_dialog);
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

void AppRunLoop()
{
    while (!app_exit)
    {
        if (Emu_IsGameRun())
            Emu_RunGame();
        else
            GUI_RunMain();
    }
}

int AppInit(int argc, char *const argv[])
{
    sceIoRemove(APP_LOG_PATH);
    sceIoRemove(CORE_LOG_PATH);
    CreateFolder(APP_DATA_DIR);

    LoadAppConfig(TYPE_CONFIG_MAIN);
    SetCurrentLang(app_config.language);

    BootCheckParams(argc, argv);

    AppLog("[INIT] Init app...\n");

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    // Init SceShellUtil events
    sceShellUtilInitEvents(0);

    // Init SceAppUtil
    initSceAppUtil();

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    checkVitatvModel();
    checkSafeMode();

    GUI_Init();

    if (is_safe_mode)
    {
        showSafeModeDialog();
        goto END;
    }

    Retro_InitLib();
    Setting_Init();

    LoadControlConfig(TYPE_CONFIG_MAIN);
    LoadHotkeyConfig(TYPE_CONFIG_MAIN);
    LoadMiscConfig(TYPE_CONFIG_MAIN);
    LoadGraphicsConfig(TYPE_CONFIG_MAIN);

    // Lock USB connection and PS button
    sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_USB_CONNECTION | SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN_2);

    // GUI_WaitInitEnd();
    GUI_StartActivity(&browser_activity);

    AppLog("[INIT] Init app OK!\n");

    if (exec_boot_mode == BOOT_MODE_GAME)
        BootLoadGame();

END:
    AppRunLoop();

    return 0;
}

int AppDeinit()
{
    Setting_Deinit();
    GUI_Deinit();
    Retro_DeinitLib();

    finishSceAppUtil();

    return 0;
}

int AppExit()
{
    app_exit = 1;
    return 0;
}
