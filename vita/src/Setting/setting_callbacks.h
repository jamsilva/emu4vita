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
static void resumeGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
}

static void resetGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ResetGame();
}

static void exitGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    if (exec_boot_mode == BOOT_MODE_GAME)
        BootReturnToParent();
}

static void exitToArchCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    BootReturnToParent();
}

static void exitAppCallback()
{
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    AppExit();
}

//----------------- State menu -----------------//
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

static void graphicsStrIndexsOptionChangedCb(StrIndexsOption *option)
{
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
}

static void graphicsStrArrayOptionChangedCb(StrArrayOption *option)
{
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
}

static void resetGraphicsConfigCallback()
{
    ResetGraphicsConfig();
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

static void controlStrArrayOptionChangedCb(StrArrayOption *option)
{
    control_option_changed = 1;
}

static void controlStrIndexsOptionChangedCb(StrIndexsOption *option)
{
    control_option_changed = 1;
}

static void controlIntStepOptionChangedCb(IntStepOption *option)
{
    control_option_changed = 1;
}

static void resetControlConfigCallback()
{
    ResetControlConfig();
    control_option_changed = 1;
    Setting_RefreshCtrlMenu();
#if defined(WSC_BUILD)
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
#endif
}

#if defined(WSC_BUILD)
static void resetVControlConfigCallback()
{
    ResetVControlConfig();
    control_option_changed = 1;
    Setting_RefreshCtrlMenu();
    if (Emu_IsGameLoaded())
        Emu_RequestRefreshVideo();
}
#endif

//----------------- Core menu -----------------//
static void coreMenuExitCallback(SettingMenu *menu)
{
    if (core_option_changed)
    {
        SaveCoreConfig(setting_config_type);
        core_option_changed = 0;
        Retro_RequestUpdateVariable();
    }
}

static void coreStrArrayOptionChangedCb(StrArrayOption *option)
{
    Retro_UpdateCoreOptionsDisplay();
    core_option_changed = 1;
}

static void resetCoreConfigCallback()
{
    ResetCoreConfig();
    core_option_changed = 1;
    Retro_UpdateCoreOptionsDisplay();
}

//----------------- Misc menu -----------------//
static void miscMenuExitCallback(SettingMenu *menu)
{
    if (misc_option_changed)
    {
        SaveMiscConfig(setting_config_type);
        misc_option_changed = 0;
    }
}

static void miscStrIndexsOptionChangedCb(StrIndexsOption *option)
{
    misc_option_changed = 1;
}

static void saveScreenshotCallback()
{
    char path[MAX_PATH_LENGTH];
    if (MakeScreenshotPath(path) < 0)
        return;
    if (Emu_SaveVideoScreenshot(path) < 0)
        return;

    GUI_CloseDialog(&setting_dialog);
}

static void saveScreenshotForPreviewCallback()
{
    char path[MAX_PATH_LENGTH];
    MakePreviewPath(path);
    if (Emu_SaveVideoScreenshot(path) < 0)
        return;

    Browser_RequestRefreshPreview(1);
    GUI_CloseDialog(&setting_dialog);
}

static void resetMiscConfigCallback()
{
    ResetMiscConfig();
    misc_option_changed = 1;
}

//----------------- App menu -----------------//
static void appMenuExitCallback(SettingMenu *menu)
{
    if (app_option_changed)
    {
        SaveAppConfig(setting_config_type);
        app_option_changed = 0;
    }
}

static void appStrIndexsOptionChangedCb(StrIndexsOption *option)
{
    app_option_changed = 1;
}

static void updatePreviewCallback(StrIndexsOption *option)
{
    Browser_RequestRefreshPreview(1);
    app_option_changed = 1;
}

static void updateAppLangCallback(StrArrayOption *option)
{
    Setting_UpdataLangOption();
    app_option_changed = 1;
}

static void resetAppConfigCallback()
{
    ResetAppConfig();
    app_option_changed = 1;
    Setting_UpdataLangOption();
    Browser_RequestRefreshPreview(1);
}

#endif