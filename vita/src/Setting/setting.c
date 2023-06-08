#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/power.h>

#include "List/config_list.h"
#include "List/option_list.h"
#include "Activity/browser.h"
#include "Retro/retro.h"
#include "Emu/emu.h"
#include "Gui/gui.h"
#include "setting.h"
#include "setting_variables.h"
#include "setting_menu.h"
#include "config.h"
#include "utils.h"
#include "file.h"
#include "lang.h"

static void refreshOptionLayout()
{
    option_listview_width = OPTION_LISTVIEW_WIDTH;
    option_listview_dx = menu_listview_dx;
    option_listview_sx = option_listview_dx - option_listview_width;
    option_listview_sy = menu_listview_sy;
    option_listview_dy = menu_listview_dy;
    option_listview_scroll_sx = option_listview_dx;
    option_listview_height = option_listview_dy - option_listview_sy;
    option_itemview_width = option_listview_width - OPTION_LISTVIEW_PADDING_L * 2;
    option_itemview_height = GUI_GetFontSize() + OPTION_ITEMVIEW_PADDING_T * 2;
    option_list_max_draw_len = (option_listview_height - OPTION_LISTVIEW_PADDING_T * 2) / option_itemview_height;

    option_scrollbar_track_x = option_listview_dx - GUI_SCROLLBAR_SIZE;
    option_scrollbar_track_y = option_listview_sy;
    option_scrollbar_track_height = option_listview_height;
}

static void refreshSettingLayout()
{
    menu_tab_view_width = WINDOW_WIDTH - WINDOW_PADDING * 2;
    menu_tab_view_height = GUI_GetFontSize() + MENU_TAB_VIEW_PADDING_T * 2;
    menu_tab_view_sx = WINDOW_SX;
    menu_tab_view_sy = WINDOW_SY;
    menu_tab_view_dx = menu_tab_view_sx + menu_tab_view_width;
    menu_tab_view_dy = menu_tab_view_sy + menu_tab_view_height;

    SETTING_FREE_DRAW_WIDTH = WINDOW_WIDTH - WINDOW_PADDING * 2;
    SETTING_FREE_DRAW_HEIGHT = WINDOW_HEIGHT - WINDOW_PADDING * 2 - menu_tab_view_height - MENU_TAB_LINEVIEW_HEIGHT;
    SETTING_FREE_DRAW_SX = WINDOW_SX;
    SETTING_FREE_DRAW_SY = menu_tab_view_dy + MENU_TAB_LINEVIEW_HEIGHT;
    SETTING_FREE_DRAW_DX = SETTING_FREE_DRAW_SX + SETTING_FREE_DRAW_WIDTH;
    SETTING_FREE_DRAW_DY = SETTING_FREE_DRAW_SY + SETTING_FREE_DRAW_HEIGHT;

    menu_listview_width = SETTING_FREE_DRAW_WIDTH;
    menu_listview_height = SETTING_FREE_DRAW_HEIGHT;
    menu_listview_sx = SETTING_FREE_DRAW_SX;
    menu_listview_sy = SETTING_FREE_DRAW_SY;
    menu_listview_dx = menu_listview_sx + menu_listview_width;
    menu_listview_dy = menu_listview_sy + menu_listview_height;
    menu_itemview_width = menu_listview_width - MENU_LISTVIEW_PADDING_L * 2;
    menu_itemview_height = GUI_GetFontSize() + MENU_ITEMVIEW_PADDING_T * 2;
    menu_list_max_draw_len = (menu_listview_height - MENU_LISTVIEW_PADDING_T * 2) / menu_itemview_height;

    menu_scrollbar_track_x = menu_listview_dx - GUI_SCROLLBAR_SIZE;
    menu_scrollbar_track_y = menu_listview_sy;
    menu_scrollbar_track_height = menu_listview_height;

    refreshOptionLayout();

    Setting_RefreshStateLayout();
}

void Setting_RequestRefreshMenu()
{
    menu_need_refresh = 1;
}

static void cleanSettingMenuItem(SettingMenuItem *item)
{
    if (item->option_type == TYPE_OPTION_STR_ARRAY)
    {
        StrArrayOption *option = (StrArrayOption *)item->option;
        if (option->names)
        {
            int i;
            for (i = 0; i < option->n_names; i++)
            {
                if (option->names[i])
                    free(option->names[i]);
            }
            free(option->names);
        }
        option->n_names = 0;
    }
}

int Setting_SetCoreMenu(OptionList *list)
{
    SettingMenu *menu;
    SettingMenuItem *item;
    StrArrayOption *option;
    OptionListEntry *entry;
    int i, j;

    // Free old menu items
    menu = &menu_list[INDEX_MENU_CORE];
    if (menu->items)
    {
        for (i = 0; i < menu->n_items; i++)
            cleanSettingMenuItem(&menu->items[i]);
        free(menu->items);
        menu->items = NULL;
    }
    menu->n_items = 0;
    menu->visible = 0;
    menu->top_pos = 0;
    menu->focus_pos = 0;

    if (!list || list->length <= 0)
        return -1;

    // Creat new menu items
    int n_items = list->length + 1;
    menu->items = (SettingMenuItem *)calloc(n_items, sizeof(SettingMenuItem));
    if (!menu->items)
        return -1;
    menu->n_items = n_items;
    menu->visible = 1;

    entry = list->head;

    for (i = 0; i < menu->n_items && entry; i++, entry = entry->next)
    {
        item = &(menu->items[i]);

        // Disable use lang
        item->lang = LANG_DISABLE;

        // Get visible pointer to option list entry for set item visible or not
        entry->visible = &(item->visible);

        // Item name
        if (entry->desc)
        {
            item->name = (char *)malloc(strlen(entry->desc) + 1);
            if (item->name)
                strcpy(item->name, entry->desc);
        }

        if (!entry->values)
            continue;

        // Item option
        option = (StrArrayOption *)calloc(1, sizeof(StrArrayOption));
        if (!option)
            continue;
        item->option_type = TYPE_OPTION_STR_ARRAY;
        item->option = option;

        // Update callback
        option->update = coreOptionChangedCallback;

        // Item option value pointer
        option->value = &(entry->sel_pos);

        // Item option names
        option->names = (char **)calloc(entry->n_values, sizeof(char *));
        if (!option->names)
            continue;
        option->n_names = entry->n_values;

        char *value;
        for (j = 0; j < option->n_names; j++)
        {
            if (entry->values[j].label)
                value = entry->values[j].label;
            else
                value = entry->values[j].value;
            if (value)
            {
                option->names[j] = (char *)malloc(strlen(value) + 1);
                if (option->names[j])
                    strcpy(option->names[j], value);
            }
        }

        item->visible = 1;
    }

    // The last one is reset config
    item = &(menu->items[n_items - 1]);
    char *name = cur_lang[RESET_CONFIGS];
    item->name = (char *)malloc(strlen(name) + 1);
    if (item->name)
        strcpy(item->name, name);
    item->option_type = TYPE_OPTION_CALLBACK;
    item->option = resetCoreConfigCallback;
    item->visible = 1;

    return 0;
}

int Setting_SetOverlayOption(OverlayList *list)
{
    int n_names = list->length + 1;
    char **names = (char **)calloc(n_names, sizeof(char *));
    if (!names)
        return -1;

    names[0] = (char *)malloc(strlen(cur_lang[NONE]) + 1);
    if (names[0])
        strcpy(names[0], cur_lang[NONE]);

    OverlayListEntry *entry = list->head;
    int i;
    for (i = 1; i < n_names && entry; i++, entry = entry->next)
    {
        if (entry->name)
        {
            names[i] = (char *)malloc(strlen(entry->name) + 1);
            if (names[i])
                strcpy(names[i], entry->name);
        }
    }

    overlay_select_option.names = names;
    overlay_select_option.n_names = n_names;

    return 0;
}

int Setting_SetAppLangOption()
{
    SetCurrentLang(app_config.app_lang);

    int n_names = 0;
    int langs_len = GetLangsLength();
    int i;

    for (i = 0; i < langs_len; i++)
    {
        if (lang_entries[i].container)
            n_names++;
    }

    char **names = (char **)calloc(n_names, sizeof(char *));
    if (!names)
        return -1;

    int index = 0;
    for (i = 0; i < langs_len; i++)
    {
        if (lang_entries[i].container)
        {
            names[index] = lang_entries[i].name;
            index++;
        }
    }

    app_lang_option.names = names;
    app_lang_option.n_names = n_names;

    return 0;
}

void Setting_UpdataLangOption()
{
    SetCurrentLang(app_config.app_lang);
    Setting_RefreshCtrlMenu();

    if (overlay_select_option.names)
    {
        if (overlay_select_option.names[0])
            free(overlay_select_option.names[0]);
        overlay_select_option.names[0] = (char *)malloc(strlen(cur_lang[NONE]) + 1);
        if (overlay_select_option.names[0])
            strcpy(overlay_select_option.names[0], cur_lang[NONE]);
    }
}

static void refreshMenuVisiblePosData()
{
    SettingMenu *menu = &menu_list[menu_list_focus_pos];
    SettingMenuItem *items = menu->items;
    int n_items = menu->n_items;
    menu_visible_pos_data.list_len = 0;

    if (!items || n_items <= 0)
        return;

    int i;
    for (i = 0; i < n_items; i++)
    {
        menu_visible_pos_data.list_indexs[i] = menu_visible_pos_data.list_len;
        if (items[i].visible)
        {
            menu_visible_pos_data.list[menu_visible_pos_data.list_len] = i;
            menu_visible_pos_data.list_len++;
        }
    }
}

static void moveMenuPos(int move_type)
{
    int visible_top_pos = menu_visible_pos_data.list_indexs[menu_top_pos];
    int visible_focus_pos = menu_visible_pos_data.list_indexs[menu_focus_pos];
    int visible_list_len = menu_visible_pos_data.list_len;

    MoveListPos(move_type, &visible_top_pos, &visible_focus_pos, visible_list_len, menu_list_max_draw_len);
    menu_top_pos = menu_visible_pos_data.list[visible_top_pos];
    menu_focus_pos = menu_visible_pos_data.list[visible_focus_pos];
}

static void moveMenuListPos(int move_type)
{
    int temp_focus_pos = menu_list_focus_pos;

    if (move_type == MOVE_TYPE_UP)
    {
        do
        {
            if (temp_focus_pos > 0)
                temp_focus_pos--;
            else
                temp_focus_pos = MENU_LIST_LEN - 1;
        } while (!menu_list[temp_focus_pos].visible);
    }
    else if (move_type == MOVE_TYPE_DOWN)
    {
        do
        {
            if (temp_focus_pos < MENU_LIST_LEN - 1)
                temp_focus_pos++;
            else
                temp_focus_pos = 0;
        } while (!menu_list[temp_focus_pos].visible);
    }

    if (temp_focus_pos != menu_list_focus_pos)
    {
        menu_list_focus_pos = temp_focus_pos;
        refreshMenuVisiblePosData();
        menu_focus_pos = 0;
        moveMenuPos(MOVE_TYPE_NONE);

        option_open = 0;
        option_listview_scroll_sx = option_listview_dx;
    }
}

static void drawOption()
{
    if (option_open)
    {
        if (option_listview_scroll_sx > option_listview_sx)
            option_listview_scroll_sx -= 22;
        if (option_listview_scroll_sx < option_listview_sx)
            option_listview_scroll_sx = option_listview_sx;
    }
    else
    {
        if (option_listview_scroll_sx < option_listview_dx)
            option_listview_scroll_sx += 22;
        if (option_listview_scroll_sx > option_listview_dx)
            option_listview_scroll_sx = option_listview_dx;
    }

    if (!option_menu || option_listview_scroll_sx >= option_listview_dx)
        return;

    GUI_DrawFillRectangle(option_listview_scroll_sx, option_listview_sy, option_listview_width, option_listview_height, COLOR_ALPHA(COLOR_BLACK, 0xBF));

    int item_sx = option_listview_scroll_sx + OPTION_LISTVIEW_PADDING_L;
    int item_dx = option_listview_scroll_sx + option_itemview_width;
    int item_sy = option_listview_sy + OPTION_LISTVIEW_PADDING_T;
    int item_max_dy = option_listview_dy - OPTION_LISTVIEW_PADDING_T;
    int clip_height = option_itemview_height;
    int mark_width = GUI_GetFontSize();
    int mark_height = mark_width;
    int mark_sx = item_dx - OPTION_ITEMVIEW_PADDING_L - mark_width;
    int i;
    for (i = option_top_pos; i < option_menu->n_items; i++)
    {
        if (item_sy >= item_max_dy)
            break;
        else if (item_sy + option_itemview_height > item_max_dy)
            clip_height = item_max_dy - item_sy;
        else
            clip_height = option_itemview_height;

        OptionMenuItem *item = &(option_menu->items[i]);

        GUI_EnableClipping();
        GUI_SetClipRectangle(item_sx, item_sy, option_itemview_width, clip_height);

        // Focus
        if (i == option_focus_pos)
            GUI_DrawFillRectangle(item_sx, item_sy, option_itemview_width, option_itemview_height, COLOR_ALPHA(COLOR_AZURE, 0xBF));

        GUI_DrawText(item_sx + OPTION_ITEMVIEW_PADDING_L, item_sy + OPTION_ITEMVIEW_PADDING_T, COLOR_GREEN, cur_lang[item->lang]);
        GUI_DrawEmptyRectangle(mark_sx, item_sy + OPTION_ITEMVIEW_PADDING_T, mark_width, mark_width, 1.0f, COLOR_GREEN);
        if (item->selected)
            GUI_DrawFillRectangle(mark_sx + 1, item_sy + OPTION_ITEMVIEW_PADDING_T + 1, mark_width - 2, mark_width - 2, COLOR_ALPHA(COLOR_ORANGE, 0xBF));

        GUI_DisableClipping();
        item_sy += option_itemview_height;
    }

    GUI_DrawVerticalScrollbar(option_scrollbar_track_x, option_scrollbar_track_y, option_scrollbar_track_height, option_menu->n_items, option_list_max_draw_len, option_top_pos, 0);
}

static void drawMenu()
{
    SettingMenu *menu = &menu_list[menu_list_focus_pos];
    SettingMenuItem *items = menu->items;
    int n_items = menu->n_items;

    // Draw items
    if (items && n_items > 0)
    {
        SettingMenuItem *item;
        int option_type;
        int item_sx = menu_listview_sx + MENU_LISTVIEW_PADDING_L;
        int item_sy = menu_listview_sy + MENU_LISTVIEW_PADDING_T;
        int item_dx = menu_listview_dx - MENU_LISTVIEW_PADDING_L;
        int item_max_dy = menu_listview_dy - MENU_LISTVIEW_PADDING_T;
        int clip_height = menu_itemview_height;
        int sx = item_sx + MENU_ITEMVIEW_PADDING_L;
        int dx = item_dx - MENU_ITEMVIEW_PADDING_L;
        int sy;

        int i;
        for (i = menu_top_pos; i < n_items; i++)
        {
            if (item_sy >= item_max_dy)
                break;
            else if (item_sy + menu_itemview_height > item_max_dy)
                clip_height = item_max_dy - item_sy;
            else
                clip_height = menu_itemview_height;

            if (!items[i].visible)
                continue;

            item = &items[i];
            option_type = item->option_type;
            sy = item_sy + MENU_ITEMVIEW_PADDING_T;

            GUI_EnableClipping();
            GUI_SetClipRectangle(item_sx, item_sy, menu_itemview_width, clip_height);

            // Focus
            if (i == menu_focus_pos)
                GUI_DrawFillRectangle(item_sx, item_sy, menu_itemview_width, menu_itemview_height, MENU_ITEMVIEW_COLOR_BG_FOCUS);

            if (item->name)
                GUI_DrawText(sx, sy, COLOR_WHITE, item->name);
            else if (item->lang >= 0)
                GUI_DrawText(sx, sy, COLOR_WHITE, cur_lang[item->lang]);

            if (item->option)
            {
                if (option_type == TYPE_OPTION_STR_ARRAY) // Option string array
                {
                    StrArrayOption *option = (StrArrayOption *)item->option;
                    int value = *(option->value);
                    const char *name = option->names[value];
                    GUI_DrawText(dx - GUI_GetTextWidth(name), sy, COLOR_GREEN, name);
                }
                else if (option_type == TYPE_OPTION_STR_INDEXS) // Option string indexs
                {
                    StrIndexsOption *option = (StrIndexsOption *)item->option;
                    int value = *(option->value);
                    const char *name = cur_lang[option->langs[value]];
                    GUI_DrawText(dx - GUI_GetTextWidth(name), sy, COLOR_GREEN, name);
                }
                else if (option_type == TYPE_OPTION_INT_ARRAY) // Option int array
                {
                    IntArrayOption *option = (IntArrayOption *)item->option;
                    int value = *(option->value);
                    const int key = option->values[value];
                    char value_str[256];
                    snprintf(value_str, 256, option->format, key);
                    GUI_DrawText(dx - GUI_GetTextWidth(value_str), sy, COLOR_GREEN, value_str);
                }
                else if (option_type == TYPE_OPTION_INT_STEP) // Option int step
                {
                    IntStepOption *option = (IntStepOption *)item->option;
                    int value = *(option->value);
                    char value_str[256];
                    snprintf(value_str, 256, option->format, value);
                    GUI_DrawText(dx - GUI_GetTextWidth(value_str), sy, COLOR_GREEN, value_str);
                }
                else if (option_type == TYPE_OPTION_KEY_MAP) // Option key map
                {
                    OptionMenu *option = (OptionMenu *)item->option;
                    int turbo = *(int *)(option->userdata) & TURBO_KEY_BITMASK;
                    uint32_t bg_color = turbo ? COLOR_ALPHA(COLOR_ORANGE, 0xBF) : COLOR_ALPHA(COLOR_BLACK, 0xBF);
                    uint32_t text_color = turbo ? COLOR_WHITE : COLOR_DARKGRAY;
                    char *turbo_str = cur_lang[TURBO];
                    int turbo_text_width = GUI_GetTextWidth(turbo_str);
                    int turbo_sx = dx - turbo_text_width;
                    GUI_DrawFillRectangle(turbo_sx, sy, turbo_text_width, GUI_GetFontSize(), bg_color);
                    GUI_DrawText(turbo_sx, sy, text_color, turbo_str);
                    GUI_DrawText(turbo_sx - GUI_GetTextWidth(option->name) - 6, sy, COLOR_GREEN, option->name);
                }
            }

            GUI_DisableClipping();
            item_sy += menu_itemview_height;
        }

        int visible_top_pos = menu_visible_pos_data.list_indexs[menu_top_pos];
        int visible_list_len = menu_visible_pos_data.list_len;
        GUI_DrawVerticalScrollbar(menu_scrollbar_track_x, menu_scrollbar_track_y, menu_scrollbar_track_height,
                                  visible_list_len, menu_list_max_draw_len, visible_top_pos, 0);

        drawOption();
    }
}

static void drawCommon()
{
    int view_sx = menu_tab_view_sx;
    int view_sy = menu_tab_view_sy;
    int view_dx = menu_tab_view_dx;
    int view_width;
    int sx;
    int sy = view_sy + MENU_TAB_VIEW_PADDING_T;

    GUI_DrawFillRectangle(WINDOW_SX, WINDOW_SY, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BG_COLOR);

    int i;
    for (i = 0; i < MENU_LIST_LEN; i++)
    {
        if (!menu_list[i].visible)
            continue;

        char *name = cur_lang[menu_list[i].lang];
        view_width = GUI_GetTextWidth(name) + MENU_TAB_VIEW_PADDING_L * 2;
        sx = view_sx + MENU_TAB_VIEW_PADDING_L;

        if (i == menu_list_focus_pos)
            GUI_DrawFillRectangle(view_sx, view_sy, view_width, menu_tab_view_height, MENU_ITEMVIEW_COLOR_BG_FOCUS);

        GUI_DrawText(sx, sy, COLOR_WHITE, name);
        view_sx += view_width;
    }

    sx = view_dx - MENU_TAB_VIEW_PADDING_L;
    if (!is_vitatv_model)
    {
        uint32_t color;
        if (scePowerIsBatteryCharging())
            color = COLOR_YELLOW;
        else if (scePowerIsLowBattery())
            color = COLOR_RED;
        else
            color = COLOR_GREEN;

        int percent = scePowerGetBatteryLifePercent();
        char battery_string[24];
        snprintf(battery_string, sizeof(battery_string), "%d%%", percent);
        int battery_x = sx - GUI_GetTextWidth(battery_string);
        GUI_DrawText(battery_x, sy, color, battery_string);
        sx = battery_x - MENU_TAB_VIEW_PADDING_L;
    }

    // time
    SceDateTime time;
    sceRtcGetCurrentClock(&time, 0);
    char time_string[16];
    GetTimeString(time_string, time_format, &time);
    int time_x = sx - GUI_GetTextWidth(time_string);
    GUI_DrawText(time_x, sy, GUI_DEFALUT_TEXT_COLOR, time_string);

    GUI_DrawFillRectangle(menu_tab_view_sx, menu_tab_view_dy, menu_tab_view_width, MENU_TAB_LINEVIEW_HEIGHT, MENU_ITEMVIEW_COLOR_BG_FOCUS);
}

static void DialogDraw()
{
    drawCommon();

    if (menu_list_focus_pos == 1)
        Setting_DrawState();
    else
        drawMenu();
}

static void ctrlCommon()
{
    if (released_pad[PAD_PSBUTTON] && IsPSbuttonEventEnabled())
    {
        if (menu_list_focus_pos != 0)
        {
            menu_list_focus_pos = 0;
            moveMenuListPos(MOVE_TYPE_NONE);
            menu_focus_pos = 0;
            refreshMenuVisiblePosData();
            moveMenuPos(MOVE_TYPE_NONE);

            option_open = 0;
            option_listview_scroll_sx = option_listview_dx;
        }
        else
        {
            GUI_CloseDialog(&setting_dialog);
        }
        return;
    }

    if (released_pad[PAD_SELECT])
    {
        if (exec_boot_mode == BOOT_MODE_ARCH && !Emu_IsGameLoaded())
            exitToArchCallback();
        else
            exitGameCallback();
        return;
    }
    else if (released_pad[PAD_START])
    {
        exitAppCallback();
        return;
    }
    else if (released_pad[PAD_L1])
    {
        moveMenuListPos(MOVE_TYPE_UP);
    }
    else if (released_pad[PAD_R1])
    {
        moveMenuListPos(MOVE_TYPE_DOWN);
    }
}

static void ctrlOption()
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        MoveListPos(MOVE_TYPE_UP, &option_top_pos, &option_focus_pos, option_menu->n_items, option_list_max_draw_len);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        MoveListPos(MOVE_TYPE_DOWN, &option_top_pos, &option_focus_pos, option_menu->n_items, option_list_max_draw_len);
    }

    if (released_pad[PAD_ENTER] || released_pad[PAD_SQUARE])
    {
        option_menu->items[option_focus_pos].selected = !option_menu->items[option_focus_pos].selected;
        if (option_menu->update)
            option_menu->update(option_menu);
    }
    else if (released_pad[PAD_TRIANGLE])
    {
        int i;
        for (i = 0; i < option_menu->n_items; i++)
        {
            option_menu->items[i].selected = 0;
        }
        if (option_menu->update)
            option_menu->update(option_menu);
    }
    else if (released_pad[PAD_CANCEL])
    {
        option_open = 0;
    }
}

static void ctrlMenu()
{
    if (option_open)
    {
        ctrlOption();
        return;
    }

    SettingMenu *menu = &menu_list[menu_list_focus_pos];
    SettingMenuItem *item = &menu->items[menu_focus_pos];
    int n_items = menu->n_items;
    int option_type = item->option_type;

    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        moveMenuPos(MOVE_TYPE_UP);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        moveMenuPos(MOVE_TYPE_DOWN);
    }

    if (released_pad[PAD_CANCEL])
    {
        GUI_CloseDialog(&setting_dialog);
    }

    if (item->option)
    {
        if (option_type == TYPE_OPTION_CALLBACK)
        {
            int (*callback)() = (int (*)())item->option;
            if (released_pad[PAD_ENTER])
            {
                if (callback)
                    callback();
            }
        }
        else if (option_type == TYPE_OPTION_STR_ARRAY)
        {
            StrArrayOption *option = (StrArrayOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->n_names - 1)
                    (*(option->value))++;
                else
                    *(option->value) = 0;
                if (option->update)
                    option->update(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > 0)
                    (*(option->value))--;
                else
                    *(option->value) = option->n_names - 1;
                if (option->update)
                    option->update(option);
            }
        }
        else if (option_type == TYPE_OPTION_STR_INDEXS)
        {
            StrIndexsOption *option = (StrIndexsOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->n_langs - 1)
                    (*(option->value))++;
                else
                    *(option->value) = 0;
                if (option->update)
                    option->update(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > 0)
                    (*(option->value))--;
                else
                    *(option->value) = option->n_langs - 1;
                if (option->update)
                    option->update(option);
            }
        }
        else if (option_type == TYPE_OPTION_INT_ARRAY)
        {
            IntArrayOption *option = (IntArrayOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->n_values - 1)
                    (*(option->value))++;
                else
                    *(option->value) = 0;
                if (option->update)
                    option->update(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > 0)
                    (*(option->value))--;
                else
                    *(option->value) = option->n_values - 1;
                if (option->update)
                    option->update(option);
            }
        }
        else if (option_type == TYPE_OPTION_INT_STEP)
        {
            IntStepOption *option = (IntStepOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->max)
                    (*(option->value))++;
                else
                    *(option->value) = option->min;
                if (option->update)
                    option->update(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > option->min)
                    (*(option->value))--;
                else
                    *(option->value) = option->max;
                if (option->update)
                    option->update(option);
            }
        }
        else if (option_type == TYPE_OPTION_KEY_MAP)
        {
            OptionMenu *option = (OptionMenu *)item->option;
            if (released_pad[PAD_ENTER])
            {
                option_menu = option;
                option_top_pos = 0;
                option_focus_pos = 0;
                if (option->open)
                    option->open(option);
                option_open = 1;
            }
            else if (released_pad[PAD_SQUARE])
            {
                uint32_t *set_key = (uint32_t *)(option->userdata);
                if (*set_key & TURBO_KEY_BITMASK)
                    *set_key &= ~TURBO_KEY_BITMASK;
                else
                    *set_key |= TURBO_KEY_BITMASK;
                control_option_changed = 1;
            }
        }
    }
}

static void DialogCtrl()
{
    ctrlCommon();

    if (menu_list_focus_pos == 1)
        Setting_CtrlState();
    else
        ctrlMenu();
}

void Setting_RefreshCtrlMenu()
{
    SettingMenu *menu = &(menu_list[INDEX_MENU_CONTROL]);
    SettingMenuItem *menu_item;

    int i, j;
    for (i = 0; i < menu->n_items; i++)
    {
        menu_item = &(menu->items[i]);
        if (menu_item->option_type != TYPE_OPTION_KEY_MAP)
            continue;

        OptionMenu *option = (OptionMenu *)(menu_item->option);
        char *text = option->name;
        uint32_t set_key = *(uint32_t *)(option->userdata);
        // ptintf("%d\n", set_key);
        int enabled = set_key & ENABLE_KEY_BITMASK;
        text[0] = '\0';
        if (!enabled)
        {
            strcpy(text, cur_lang[DISABLE]);
            continue;
        }

        int n_maped = 0;
        for (j = 0; j < option->n_items; j++)
        {
            OptionMenuItem *option_item = &(option->items[j]);
            uint32_t map_key = (uint32_t)(option_item->userdata);

            if (set_key & map_key)
            {
                if (n_maped > 0)
                    strcat(text, "+");
                strcat(text, cur_lang[option_item->lang]);
                n_maped++;
            }
        }
    }
}

static int DialogOpen()
{
    Setting_WaitOverlayInitEnd();

    if (Emu_IsGameLoaded())
        setting_config_type = TYPE_CONFIG_GAME;
    else
        setting_config_type = TYPE_CONFIG_MAIN;

    if (CurrentPathIsFile())
    {
        Setting_InitState();
        menu_list[INDEX_MENU_STATE].visible = 1;
    }
    else
    {
        menu_list[INDEX_MENU_STATE].visible = 0;
        if (menu_list_focus_pos == 1)
        {
            menu_list_focus_pos = 0;
            menu_need_refresh = 1;
        }
    }

    if (menu_need_refresh)
    {
        if (Emu_IsGameLoaded())
        {
            // 程序菜单 (菜单 可见/隐藏)
            menu_list[INDEX_MENU_APP].visible = 0;
            // 核心菜单 (菜单 可见/隐藏)
            if (menu_list[INDEX_MENU_CORE].items)
                menu_list[INDEX_MENU_CORE].visible = 1;
            else
                menu_list[INDEX_MENU_CORE].visible = 0;
            // 主菜单 (子选项 可见/隐藏)
            main_menu_items[0].visible = 1; // 继续游戏
            main_menu_items[1].visible = 1; // 重置游戏
            main_menu_items[2].visible = 1; // 退出游戏
            // 杂项菜单 (子选项 可见/隐藏)
            misc_menu_items[2].visible = 1; // 保存截图
            misc_menu_items[3].visible = 1; // 保存截图为预览图
        }
        else
        {
            // 程序菜单 (菜单 可见/隐藏)
            menu_list[INDEX_MENU_APP].visible = 1;
            // 核心菜单 (菜单 可见/隐藏)
            if (menu_list[INDEX_MENU_CORE].items && core_menu_for_main_enabled)
                menu_list[INDEX_MENU_CORE].visible = 1;
            else
                menu_list[INDEX_MENU_CORE].visible = 0;
            // 主菜单 (子选项 可见/隐藏)
            main_menu_items[0].visible = 0;
            main_menu_items[1].visible = 0;
            main_menu_items[2].visible = 0;
            // 杂项菜单 (子选项 可见/隐藏)
            misc_menu_items[2].visible = 0;
            misc_menu_items[3].visible = 0;
        }

        if (exec_boot_mode == BOOT_MODE_ARCH)
            main_menu_items[3].visible = 1; // 返回模拟器前端
        else
            main_menu_items[3].visible = 0; // 返回模拟器前端

        if (is_vitatv_model)
        {
            control_menu_items[18].visible = 0; // 前触摸补键
            control_menu_items[19].visible = 0; // 背触摸补键
        }
        else
        {
            control_menu_items[18].visible = 1; // 前触摸补键
            control_menu_items[19].visible = 1; // 背触摸补键
        }

        Setting_RefreshCtrlMenu();
        Settting_SetStateSelectId(0);
        menu_list_focus_pos = 0;
        moveMenuListPos(MOVE_TYPE_NONE);
        menu_focus_pos = 0;
        refreshMenuVisiblePosData();
        moveMenuPos(MOVE_TYPE_NONE);
        menu_need_refresh = 0;
    }

    return 0;
}

static int DialogClose()
{
    int i;
    for (i = 0; i < MENU_LIST_LEN; i++)
    {
        if (menu_list[i].visible && menu_list[i].exit)
            menu_list[i].exit(&menu_list[i]);
    }

    if (Emu_IsGameLoaded())
        Emu_ResumeGame();

    return 0;
}

int Setting_Init()
{
    if (menu_list[INDEX_MENU_CORE].items)
        core_menu_for_main_enabled = 1;
    Setting_SetAppLangOption();
    Setting_InitOverlay();
    refreshSettingLayout();
    return 0;
}

int Setting_Deinit()
{
    return 0;
}