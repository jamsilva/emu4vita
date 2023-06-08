#ifndef __M_SETTING_H__
#define __M_SETTING_H__

#include <stdint.h>

#include "List/option_list.h"
#include "List/overlay_list.h"
#include "setting_state.h"
#include "setting_overlay.h"

enum TypeSettingOption
{
    TYPE_OPTION_NONE,
    TYPE_OPTION_CALLBACK,
    TYPE_OPTION_STR_ARRAY,
    TYPE_OPTION_STR_INDEXS,
    TYPE_OPTION_INT_ARRAY,
    TYPE_OPTION_INT_STEP,
    TYPE_OPTION_KEY_MAP,
};

typedef struct StrArrayOption
{
    uint32_t *value;
    char **names;
    int n_names;
    void (*update)(struct StrArrayOption *option);
    void *userdata;
} StrArrayOption;

typedef struct StrIndexsOption
{
    uint32_t *value;
    int *langs;
    int n_langs;
    void (*update)(struct StrIndexsOption *option);
    void *userdata;
} StrIndexsOption;

typedef struct IntArrayOption
{
    uint32_t *value;
    int *values;
    int n_values;
    char *format;
    void (*update)(struct IntArrayOption *option);
    void *userdata;
} IntArrayOption;

typedef struct IntStepOption
{
    int32_t *value;
    int32_t min;
    int32_t max;
    int32_t step;
    char *format;
    void (*update)(struct IntStepOption *option);
    void *userdata;
} IntStepOption;

typedef struct
{
    int lang;
    void *userdata;
    int selected;
} OptionMenuItem;

typedef struct OptionMenu
{
    char *name;
    OptionMenuItem *items;
    int n_items;
    void (*open)(struct OptionMenu *option);
    void (*update)(struct OptionMenu *option);
    void *userdata;
} OptionMenu;

typedef struct SettingMenuItem
{
    int lang;
    char *name;
    int visible;
    int option_type;
    void *option;
} SettingMenuItem;

typedef struct SettingMenu
{
    int lang;
    int visible;
    SettingMenuItem *items;
    int n_items;
    int top_pos;
    int focus_pos;
    void (*init)(struct SettingMenu *menu);
    void (*exit)(struct SettingMenu *menu);
} SettingMenu;

extern int SETTING_FREE_DRAW_WIDTH, SETTING_FREE_DRAW_HEIGHT;
extern int SETTING_FREE_DRAW_SX, SETTING_FREE_DRAW_SY, SETTING_FREE_DRAW_DX, SETTING_FREE_DRAW_DY;

int Setting_Init();
int Setting_Deinit();

int Setting_SetCoreMenu(OptionList *list);
int Setting_SetOverlayOption(OverlayList *list);
int Setting_SetAppLangOption();
void Setting_UpdataLangOption();

void Setting_RefreshCtrlMenu();
void Setting_RequestRefreshMenu();
void Setting_RequestRefreshOptionDisplay();

#endif