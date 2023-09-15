#ifndef __M_SETTING_H__
#define __M_SETTING_H__

#include "list/option_list.h"
#include "list/overlay_list.h"
#include "setting_state.h"
#include "setting_overlay.h"
#include "lang.h"

enum TypeSettingOption
{
    TYPE_OPTION_NONE,
    TYPE_OPTION_CALLBACK,
    TYPE_OPTION_STR_ARRAY,
    TYPE_OPTION_INT_ARRAY,
    TYPE_OPTION_INT_RANGE,
    TYPE_OPTION_CHECK_BOX,
};

typedef struct StrArrayOption
{
    uint32_t *value;
    LangString *names;
    int n_names;
    void (*updateCallback)(struct StrArrayOption *option);
    void *userdata;
} StrArrayOption;

typedef struct IntArrayOption
{
    uint32_t *value;
    int *values;
    int n_values;
    char *format;
    void (*updateCallback)(struct IntArrayOption *option);
    void *userdata;
} IntArrayOption;

typedef struct IntRangeOption
{
    int32_t *value;
    int32_t min;
    int32_t max;
    int32_t step;
    char *format;
    void (*updateCallback)(struct IntRangeOption *option);
    void *userdata;
} IntRangeOption;

typedef struct CheckBoxOptionMenuItem
{
    LangString name;
    int selected;
    void *userdata;
} CheckBoxOptionMenuItem;

typedef struct CheckBoxOptionMenu
{
    LangString name;
    CheckBoxOptionMenuItem *items;
    int n_items;
    void (*openCallback)(struct CheckBoxOptionMenu *option);
    void (*closeCallback)(struct CheckBoxOptionMenu *option);
    void (*updateCallback)(struct CheckBoxOptionMenu *option);
    void *userdata;
} CheckBoxOptionMenu;

typedef struct SettingMenuItem
{
    LangString name;
    int *visibility;
    int option_type;
    void *option;
} SettingMenuItem;

typedef struct SettingMenu
{
    LangString name;
    int *visibility;
    SettingMenuItem *items;
    int n_items;
    void (*enterCallback)(struct SettingMenu *menu);
    void (*exitCallback)(struct SettingMenu *menu);
} SettingMenu;

extern int SETTING_FREE_DRAW_WIDTH, SETTING_FREE_DRAW_HEIGHT;
extern int SETTING_FREE_DRAW_SX, SETTING_FREE_DRAW_SY, SETTING_FREE_DRAW_DX, SETTING_FREE_DRAW_DY;

int Setting_Init();
int Setting_Deinit();

int Setting_SetCoreMenu(LinkedList *list);
int Setting_SetOverlayOption(LinkedList *list);
int Setting_SetLangOption();
void Setting_UpdataLangOption();

void Setting_UpdateKeyMapperMenu(int idx);
void Setting_RequestUpdateMenu();
void Setting_RequestUpdateMenuItems();

#endif