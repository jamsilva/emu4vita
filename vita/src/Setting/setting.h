#ifndef __M_SETTING_H__
#define __M_SETTING_H__

#include <stdint.h>

#include "List/option_list.h"
#include "List/overlay_list.h"
#include "setting_state.h"
#include "setting_overlay.h"

enum SettingMenuItemTypes
{
    OPTION_TYPE_NONE,
    OPTION_TYPE_CALLBACK,
    OPTION_TYPE_MENU,
    OPTION_TYPE_STR,
    OPTION_TYPE_STR_ARRAY,
    OPTION_TYPE_INT,
    OPTION_TYPE_INT_ARRAY,
    OPTION_TYPE_INT_STEP,
    OPTION_TYPE_CALLBACK_VOID,
    OPTION_TYPE_CHECK_BOX,
    OPTION_TYPE_KEY_MAP,
};

typedef struct StrArrayOption
{
    uint32_t *value;
    char **names;
    int n_names;
    void (*update)(struct StrArrayOption *option);
    void *userdata;
} StrArrayOption;

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
    char *name;
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
    char *name;
    int visible;
    int option_type;
    void *option;
} SettingMenuItem;

typedef struct SettingMenu
{
    char *name;
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

void Setting_RefreshCtrlMenu();
void Setting_RequestRefreshMenu();

#endif