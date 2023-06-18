#ifndef __M_SETTING_H__
#define __M_SETTING_H__

#include <stdint.h>

#include "list/option_list.h"
#include "list/overlay_list.h"
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
    TYPE_OPTION_KEY_MAPPER,
};

typedef struct StrArrayOption
{
    uint32_t *value;
    char **names;
    int n_names;
    void (*updateCallback)(struct StrArrayOption *option);
    void *userdata;
} StrArrayOption;

typedef struct StrIndexsOption
{
    uint32_t *value;
    int *langs;
    int n_langs;
    void (*updateCallback)(struct StrIndexsOption *option);
    void *userdata;
} StrIndexsOption;

typedef struct IntArrayOption
{
    uint32_t *value;
    int *values;
    int n_values;
    char *format;
    void (*updateCallback)(struct IntArrayOption *option);
    void *userdata;
} IntArrayOption;

typedef struct IntStepOption
{
    int32_t *value;
    int32_t min;
    int32_t max;
    int32_t step;
    char *format;
    void (*updateCallback)(struct IntStepOption *option);
    void *userdata;
} IntStepOption;

typedef struct KeyMapperOptionMenuItem
{
    int lang;
    void *userdata;
    int selected;
} KeyMapperOptionMenuItem;

typedef struct KeyMapperOptionMenu
{
    char *name;
    KeyMapperOptionMenuItem *items;
    int n_items;
    void (*openCallback)(struct KeyMapperOptionMenu *option);
    void (*closeCallback)(struct KeyMapperOptionMenu *option);
    void (*updateCallback)(struct KeyMapperOptionMenu *option);
    void *userdata;
} KeyMapperOptionMenu;

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
    void (*enterCallback)(struct SettingMenu *menu);
    void (*exitCallback)(struct SettingMenu *menu);
} SettingMenu;

extern int SETTING_FREE_DRAW_WIDTH, SETTING_FREE_DRAW_HEIGHT;
extern int SETTING_FREE_DRAW_SX, SETTING_FREE_DRAW_SY, SETTING_FREE_DRAW_DX, SETTING_FREE_DRAW_DY;

int Setting_Init();
int Setting_Deinit();

int Setting_SetCoreMenu(OptionList *list);
int Setting_SetOverlayOption(OverlayList *list);
int Setting_SetAppLangOption();
void Setting_UpdataLangOption();

void Setting_UpdateKeyMapperMenu(int idx);
void Setting_PushUpdateMenu();
void Setting_PushUpdateOptionDisplay();

#endif