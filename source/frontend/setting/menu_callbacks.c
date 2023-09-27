#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "activity/browser.h"
#include "emu/emu.h"
#include "setting.h"
#include "config.h"
#include "boot.h"
#include "init.h"
#include "lang.h"
#include "file.h"

#ifndef __SETTING_MENU_EMBED
#include "setting_variables.c"
#include "menu_keymapper.c"
#endif

static void openKeyMapperOptionCallback(CheckBoxOptionMenu *option);

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
    Emu_DiskChangeImageIndex(data->focus_pos);
    GUI_CloseAllDialogs(TYPE_GUI_DIALOG_ANY);
}

static void diskControlCallback()
{
    GUI_Dialog *dialog = AlertDialog_Create();
    AlertDialog_SetTitle(dialog, cur_lang[TITLE_SWITCH_DISK]);
    int n_items = Emu_DiskGetNumImages();
    char **items = (char **)malloc(n_items * sizeof(char *));
    char string[MAX_NAME_LENGTH];
    int cur_index = Emu_DiskGetImageIndex();

    int i;
    for (i = 0; i < n_items; i++)
    {
        if (i == cur_index)
            snprintf(string, MAX_NAME_LENGTH, "%s %d (%s)", cur_lang[LABEL_DISK], i + 1, cur_lang[CURRENT]);
        else
            snprintf(string, MAX_NAME_LENGTH, "%s %d", cur_lang[LABEL_DISK], i + 1);
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
static void stateMenuEnterCallback(SettingMenu *menu)
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

static void graphicsOptionUpdateCallback(StrArrayOption *option)
{
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestUpdateVideoDisplay();
}

static void resetGraphicsConfigCallback()
{
    ResetGraphicsConfig();
    graphics_option_changed = 1;
    if (Emu_IsGameLoaded())
        Emu_RequestUpdateVideoDisplay();
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

static void controlOptionUpdateCallback(void *option)
{
    control_option_changed = 1;
}

static void controlKeymapperOptionChangedCallback(CheckBoxOptionMenu *option)
{
    updateKeyMapperOptionCallback(option);
    control_option_changed = 1;
}

static void resetControlConfigCallback()
{
    ResetControlConfig();
    control_option_changed = 1;
    Setting_UpdateKeyMapperMenu(INDEX_MENU_CONTROL);
}

//----------------- Hotkey menu -----------------//
static void hotkeyMenuExitCallback(SettingMenu *menu)
{
    if (hotkey_option_changed)
    {
        SaveHotkeyConfig(setting_config_type);
        hotkey_option_changed = 0;
    }
}

static void hotkeyOptionUpdateCallback(CheckBoxOptionMenu *option)
{
    updateKeyMapperOptionCallback(option);
    hotkey_option_changed = 1;
}

static void resetHotkeyConfigCallback()
{
    ResetHotkeyConfig(setting_config_type);
    hotkey_option_changed = 1;
    Setting_UpdateKeyMapperMenu(INDEX_MENU_HOTKEY);
}

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

static void coreOptionUpdateCallback(StrArrayOption *option)
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

static void miscOptionUpdateCallback(StrArrayOption *option)
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

static void appOptionUpdateCallback(StrArrayOption *option)
{
    app_option_changed = 1;
}

static void previewOptionUpdateCallback(StrArrayOption *option)
{
    Browser_RequestRefreshPreview(1);
    app_option_changed = 1;
}

static void langOptionUpdateCallback(StrArrayOption *option)
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
