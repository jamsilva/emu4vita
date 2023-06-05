#ifndef __M_SETTING_CALLBACK_H__
#define __M_SETTING_CALLBACK_H__

#include "Activity/browser.h"
#include "Emu/emu.h"
#include "setting.h"
#include "setting_variables.h"
#include "config.h"
#include "boot.h"
#include "init.h"
#include "lang.h"

extern GUI_Dialog setting_dialog;

static int app_option_changed = 0;
static int graphics_option_changed = 0;
static int control_option_changed = 0;
static int core_option_changed = 0;
static int misc_option_changed = 0;

//----------------- Main menu -----------------//

static int resumeGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
    return 0;
}

static int resetGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ResetGame();
    return 0;
}

static int exitGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    if (exec_boot_mode == BOOT_MODE_GAME)
        BootReturnToParent();
    return 0;
}

static int exitToArchCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    BootReturnToParent();
    return 0;
}

static int exitAppCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    AppExit();
    return 0;
}

//----------------- State menu -----------------//

static void stateMenuOpenCallback(SettingMenu *menu)
{
    Setting_InitState();
}

static void stateMenuExitCallback(SettingMenu *menu)
{
    Setting_DeinitState();
}

//----------------- Graphics menu -----------------//

static void graphicsMenuExitCallback(SettingMenu *menu)
{
    if (graphics_option_changed)
    {
        graphics_option_changed = 0;
        SaveGraphicsConfig(setting_config_type);
    }
}

static int resetGraphicsConfigCallback()
{
    ResetGraphicsConfig();
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
    graphics_option_changed = 1;
    return 0;
}

static void graphicsOptionChangedCallback(StrArrayOption *option)
{
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
}

//----------------- Control menu -----------------//

static void controlMenuExitCallback(SettingMenu *menu)
{
    if (control_option_changed)
    {
        SaveControlConfig(setting_config_type);
        control_option_changed = 0;
    }
}

static int resetControlConfigCallback()
{
    ResetControlConfig();
    Setting_RefreshCtrlMenu();
    control_option_changed = 1;
    return 0;
}

#if defined(WSC_BUILD)
static int resetVControlConfigCallback()
{
    ResetVControlConfig();
    Setting_RefreshCtrlMenu();
    control_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
    return 0;
}
#endif

static void controlOptionChangedCallback(StrArrayOption *option)
{
    control_option_changed = 1;
}

//----------------- Core menu -----------------//

static void coreMenuExitCallback(SettingMenu *menu)
{
    if (core_option_changed)
    {
        core_option_changed = 0;
        SaveCoreConfig(setting_config_type);
        Retro_RequestUpdateVariable();
    }
}

static int resetCoreConfigCallback()
{
    ResetCoreConfig();
    core_option_changed = 1;
    Retro_UpdateCoreOptionsDisplay();
    return 0;
}

static void coreOptionChangedCallback(StrArrayOption *option)
{
    core_option_changed = 1;
    Retro_UpdateCoreOptionsDisplay();
}

//----------------- Misc menu -----------------//

static void miscMenuExitCallback(SettingMenu *menu)
{
    if (misc_option_changed)
    {
        misc_option_changed = 0;
        SaveMiscConfig(setting_config_type);
    }
}

static int deleteAutoStateCallback()
{
    Emu_DeleteState(-1);
    Browser_RequestRefreshPreview(1);
    return 0;
}

static int saveScreenshotCallback()
{
    char path[MAX_PATH_LENGTH];
    if (MakeScreenshotPath(path) < 0)
        return -1;
    if (Emu_SaveVideoScreenshot(path) < 0)
        return -1;

    GUI_CloseDialog(&setting_dialog);
    return 0;
}

static int saveScreenshotForPreviewCallback()
{
    char path[MAX_PATH_LENGTH];
    MakePreviewPath(path);
    if (Emu_SaveVideoScreenshot(path) < 0)
        return -1;

    Browser_RequestRefreshPreview(1);
    GUI_CloseDialog(&setting_dialog);
    return 0;
}

static int resetMiscConfigCallback()
{
    ResetMiscConfig();
    misc_option_changed = 1;
    return 0;
}

static void miscOptionChangedCallback(StrArrayOption *option)
{
    misc_option_changed = 1;
}

//----------------- App menu -----------------//

static void appMenuExitCallback(SettingMenu *menu)
{
    if (app_option_changed)
    {
        app_option_changed = 0;
        SaveAppConfig(setting_config_type);
    }
}

static int resetAppConfigCallback()
{
    ResetAppConfig();
    Browser_RequestRefreshPreview(1);
    app_option_changed = 1;
    return 0;
}

static void updatePreviewCallback(StrArrayOption *option)
{
    Browser_RequestRefreshPreview(1);
    app_option_changed = 1;
}

static void appOptionChangedCallback(StrArrayOption *option)
{
    app_option_changed = 1;
}

#endif