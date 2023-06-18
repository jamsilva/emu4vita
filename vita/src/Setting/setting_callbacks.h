#ifndef __M_SETTING_CALLBACK_H__
#define __M_SETTING_CALLBACK_H__

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "activity/browser.h"
#include "emu/emu.h"
#include "setting.h"
#include "setting_variables.h"
#include "config.h"
#include "boot.h"
#include "init.h"
#include "lang.h"
#include "file.h"

//----------------- Main menu -----------------//
static void resumeGameCallback()
{
    GUI_CloseDialog(&setting_dialog);
}

static void resetGameCallback()
{
    Emu_CleanAudioSound();
    GUI_CloseDialog(&setting_dialog);
    Emu_ResetGame();
}

static void exitGameCallback()
{
    Emu_CleanAudioSound();
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    if (exec_boot_mode == BOOT_MODE_GAME)
        BootReturnToParent();
}

static void diskControlOptionMenuPositiveCallback(GUI_Dialog *dialog)
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    int cur_index = Emu_DiskGetImageIndex();
    if (data->focus_pos == cur_index)
    {
        AlertDialog_ShowSimpleTipDialog(cur_lang[TITLE_TIP], "已在当前光盘，无需更换");
        return;
    }

    Emu_DiskChangeImageIndex(data->focus_pos);
    GUI_CloseAllDialogs(TYPE_GUI_DIALOG_ANY);
}

static void diskControlCallback()
{
    GUI_Dialog *dialog = AlertDialog_Creat();
    AlertDialog_SetTitle(dialog, "更换光盘");
    int n_items = Emu_DiskGetNumImages();
    char **items = (char **)malloc(n_items * sizeof(char *));
    char string[MAX_NAME_LENGTH];

    int i;
    for (i = 0; i < n_items; i++)
    {
        snprintf(string, MAX_NAME_LENGTH, "光盘 %d", i + 1);
        items[i] = (char *)malloc(strlen(string) + 1);
        strcpy(items[i], string);
    }

    AlertDialog_SetItems(dialog, items, n_items);
    AlertDialog_SetPositiveButton(dialog, cur_lang[CONFIRM], diskControlOptionMenuPositiveCallback);
    AlertDialog_SetNegativeButton(dialog, cur_lang[CANCEL], NULL);
    AlertDialog_Show(dialog);
}

static void exitToArchCallback()
{
    Emu_CleanAudioSound();
    GUI_CloseDialog(&setting_dialog);
    Emu_ExitGame();
    BootReturnToParent();
}

static void exitAppCallback()
{
    Emu_CleanAudioSound();
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
        Emu_PushUpdateVideoDisplay();
}

static void graphicsStrArrayOptionChangedCb(StrArrayOption *option)
{
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_PushUpdateVideoDisplay();
}

static void resetGraphicsConfigCallback()
{
    ResetGraphicsConfig();
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_PushUpdateVideoDisplay();
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
    Setting_UpdateKeyMapperMenu(INDEX_MENU_CONTROL);
#if defined(WSC_BUILD)
    if (Emu_IsGameLoaded())
        Emu_PushUpdateVideoDisplay();
#endif
}

#if defined(WSC_BUILD)
static void resetVControlConfigCallback()
{
    ResetVControlConfig();
    control_option_changed = 1;
    Setting_UpdateKeyMapperMenu(INDEX_MENU_CONTROL);
    if (Emu_IsGameLoaded())
        Emu_PushUpdateVideoDisplay();
}
#endif

//----------------- Core menu -----------------//
static void coreMenuExitCallback(SettingMenu *menu)
{
    if (core_option_changed)
    {
        SaveCoreConfig(setting_config_type);
        core_option_changed = 0;
        Retro_PushUpdateVariable();
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

    Browser_PushRefreshPreview(1);
    GUI_CloseDialog(&setting_dialog);
}

static void resetMiscConfigCallback()
{
    ResetMiscConfig();
    Setting_UpdateKeyMapperMenu(INDEX_MENU_MISC);
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
    Browser_PushRefreshPreview(1);
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
    Browser_PushRefreshPreview(1);
}

#endif