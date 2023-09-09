#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/power.h>

#include "list/config_list.h"
#include "list/option_list.h"
#include "activity/browser.h"
#include "emu/emu.h"
#include "gui/gui.h"
#include "setting.h"
#include "config.h"
#include "utils.h"
#include "file.h"
#include "lang.h"
#include "setting_variables.c"
#define __SETTING_MENU_EMBED
#include "setting_menu.c"

static int openDialogCallback(GUI_Dialog *dialog);
static int closeDialogCallback(GUI_Dialog *dialog);
static void drawDialogCallback(GUI_Dialog *dialog);
static void ctrlDialogCallback(GUI_Dialog *dialog);

GUI_Dialog setting_dialog = {
    TYPE_GUI_DIALOG_WINDOW, // Dialog type
    openDialogCallback,     // Open callback
    closeDialogCallback,    // Close callback
    drawDialogCallback,     // Draw callback
    ctrlDialogCallback,     // Ctrl callback
    NULL,                   // Previous dialog
    NULL,                   // Next dialog
    NULL,                   // User data
};

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
    option_listview_n_draw_items = (option_listview_height - OPTION_LISTVIEW_PADDING_T * 2) / option_itemview_height;

    option_scrollbar_track_x = option_listview_dx - GUI_DEF_SCROLLBAR_SIZE - 2;
    option_scrollbar_track_y = option_listview_sy + 2;
    option_scrollbar_track_height = option_listview_height + 4;
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
    menu_listview_n_draw_items = (menu_listview_height - MENU_LISTVIEW_PADDING_T * 2) / menu_itemview_height;

    menu_scrollbar_track_x = menu_listview_dx - GUI_DEF_SCROLLBAR_SIZE - 2;
    menu_scrollbar_track_y = menu_listview_sy + 2;
    menu_scrollbar_track_height = menu_listview_height - 4;

    refreshOptionLayout();

    Setting_RefreshStateLayout();
}

void Setting_RequestUpdateMenu()
{
    menu_need_update = 1;
}

void Setting_RequestUpdateMenuItems()
{
    menu_items_need_update = 1;
}

static void refreshSelectMenuItems()
{
    if (select_menu_items)
        free(select_menu_items);
    select_menu_items = NULL;
    n_select_menu_items = 0;

    SettingMenuItem *items = menu_list[menu_list_focus_pos]->items;
    int n_items = menu_list[menu_list_focus_pos]->n_items;
    int n_visibles = 0;
    int i;

    for (i = 0; i < n_items; i++)
    {
        if (*items[i].visibility)
            n_visibles++;
    }

    if (n_visibles == 0)
        return;

    select_menu_items = (SettingMenuItem **)malloc(n_visibles * sizeof(SettingMenuItem *));
    if (!select_menu_items)
        return;

    for (i = 0; i < n_items; i++)
    {
        if (*items[i].visibility)
            select_menu_items[n_select_menu_items++] = &items[i];
    }
}

static void setFocusByName(const char *name)
{
    if (!name)
        return;

    int pos = -1;
    int i;
    for (i = 0; i < n_select_menu_items; i++)
    {
        if (select_menu_items[i]->name && strcmp(select_menu_items[i]->name, name) == 0)
        {
            pos = i;
            break;
        }
    }
    if (pos == -1)
        return;

    menu_focus_pos = pos;
    RefreshListPos(&menu_top_pos, &menu_focus_pos, n_select_menu_items, menu_listview_n_draw_items);
}

static void moveMenuListPos(int move_type)
{
    int pos = menu_list_focus_pos;

    if (move_type == TYPE_MOVE_UP)
    {
        do
        {
            if (pos > 0)
                pos--;
            else
                pos = MENU_LIST_LEN - 1;
        } while (!*menu_list[pos]->visibility && pos != menu_list_focus_pos);
    }
    else if (move_type == TYPE_MOVE_DOWN)
    {
        do
        {
            if (pos < MENU_LIST_LEN - 1)
                pos++;
            else
                pos = 0;
        } while (!*menu_list[pos]->visibility && pos != menu_list_focus_pos);
    }

    menu_list_focus_pos = pos;
    menu_top_pos = 0;
    menu_focus_pos = 0;
    refreshSelectMenuItems();
    option_open = 0;
    option_listview_scroll_sx = option_listview_dx;
}

static void cleanSettingMenuItem(SettingMenuItem *item)
{
    if (item->option_type == TYPE_OPTION_STR_ARRAY)
    {
        StrArrayOption *option = (StrArrayOption *)item->option;
        if (option->langs)
            free(option->langs);
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
        option->langs = NULL;
        option->names = NULL;
        option->n_names = 0;
    }
}

static void destroySettingMenuItems(SettingMenuItem *items, int n_items)
{
    int i;
    for (i = 0; i < n_items; i++)
    {
        cleanSettingMenuItem(&items[i]);
    }
    free(items);
}

int Setting_SetCoreMenu(LinkedList *list)
{
    // Free old menu items
    if (core_menu.items)
        destroySettingMenuItems(core_menu.items, core_menu.n_items);
    core_menu.items = NULL;
    core_menu.n_items = 0;

    if (!list)
        return -1;

    int l_length = LinkedListGetLength(list);

    // Creat new menu items
    int n_items = l_length + 1; // +1 for reset config
    SettingMenuItem *items = (SettingMenuItem *)calloc(n_items, sizeof(SettingMenuItem));
    if (!items)
        return -1;

    LinkedListEntry *entry = LinkedListHead(list);

    int i, j;
    for (i = 0; i < l_length && entry; i++)
    {
        OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);

        // Disable use lang
        items[i].lang = LANG_NULL;
        items[i].visibility = &(data->visibility);

        // Item name
        char *desc = data->desc;
        if (!desc)
            desc = data->key;
        if (desc)
        {
            // printf("desc: %s\n", desc);
            items[i].name = (char *)malloc(strlen(desc) + 1);
            if (items[i].name)
                strcpy(items[i].name, desc);
        }

        if (!data->values || data->n_values <= 0)
            continue;

        // Item option
        StrArrayOption *option = (StrArrayOption *)calloc(1, sizeof(StrArrayOption));
        if (!option)
            continue;
        items[i].option_type = TYPE_OPTION_STR_ARRAY;
        items[i].option = option;

        // Update callback
        option->updateCallback = coreOptionUpdateCallback;

        // Item option value pointer
        option->value = &(data->select);

        // Item option names
        option->names = (char **)calloc(data->n_values, sizeof(char *));
        if (!option->names)
            continue;
        option->n_names = data->n_values;

        for (j = 0; j < option->n_names; j++)
        {
            char *value;
            if (data->values[j].label)
                value = data->values[j].label;
            else
                value = data->values[j].value;
            if (value)
            {
                option->names[j] = (char *)malloc(strlen(value) + 1);
                if (option->names[j])
                    strcpy(option->names[j], value);
            }
        }

        entry = LinkedListNext(entry);
    }

    // The last one is reset config
    items[n_items - 1].lang = LABEL_RESET_CONFIGS;
    items[n_items - 1].option_type = TYPE_OPTION_CALLBACK;
    items[n_items - 1].option = resetCoreConfigCallback;
    items[n_items - 1].visibility = &visibility_visible;

    core_menu.items = items;
    core_menu.n_items = n_items;

    if (menu_list_focus_pos == INDEX_MENU_CORE)
        moveMenuListPos(TYPE_MOVE_NONE);

    return 0;
}

int Setting_SetOverlayOption(LinkedList *list)
{
    if (!list)
        return -1;

    int l_length = LinkedListGetLength(list);
    printf("overlay: length = %d\n", l_length);

    int n_names = l_length + 1; // +1 for none
    char **names = (char **)calloc(n_names, sizeof(char *));
    if (!names)
        return -1;

    names[0] = (char *)malloc(strlen(cur_lang[NONE]) + 1);
    if (names[0])
        strcpy(names[0], cur_lang[NONE]);

    LinkedListEntry *entry = LinkedListHead(list);

    int i;
    for (i = 1; i < n_names && entry; i++)
    {
        OverlayListEntryData *data = (OverlayListEntryData *)LinkedListGetEntryData(entry);
        if (data->name)
        {
            printf("overlay: name = %s\n", data->name);
            names[i] = (char *)malloc(strlen(data->name) + 1);
            if (names[i])
                strcpy(names[i], data->name);
        }
        entry = LinkedListNext(entry);
    }

    overlay_select_option.names = names;
    overlay_select_option.n_names = n_names;

    return 0;
}

int Setting_SetAppLangOption()
{
    SetCurrentLang(app_config.language);

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

    language_option.names = names;
    language_option.n_names = n_names;

    return 0;
}

void Setting_UpdataLangOption()
{
    SetCurrentLang(app_config.language);
    Setting_UpdateKeyMapperMenu(INDEX_MENU_CONTROL);
    Setting_UpdateKeyMapperMenu(INDEX_MENU_HOTKEY);

    if (overlay_select_option.names)
    {
        if (overlay_select_option.names[0])
            free(overlay_select_option.names[0]);
        overlay_select_option.names[0] = (char *)malloc(strlen(cur_lang[NONE]) + 1);
        if (overlay_select_option.names[0])
            strcpy(overlay_select_option.names[0], cur_lang[NONE]);
    }
}

void Setting_UpdateKeyMapperMenu(int idx)
{
    SettingMenuItem *items = menu_list[idx]->items;
    int n_items = menu_list[idx]->n_items;

    int i, j;
    for (i = 0; i < n_items; i++)
    {
        if (items[i].option_type != TYPE_OPTION_CHECK_BOX)
            continue;

        CheckBoxOptionMenu *option = (CheckBoxOptionMenu *)(items[i].option);
        char *text = option->name;
        uint32_t config_key = *(uint32_t *)(option->userdata);
        // ptintf("%d\n", config_key);
        int enabled = config_key & ENABLE_KEY_BITMASK;
        text[0] = '\0';
        if (!enabled)
        {
            strcpy(text, cur_lang[DISABLE]);
            continue;
        }

        int n_maped = 0;
        for (j = 0; j < option->n_items; j++)
        {
            CheckBoxOptionMenuItem *option_item = &(option->items[j]);
            uint32_t mapping_key = (uint32_t)(option_item->userdata);
            char *name = option_item->name;
            if (option_item->lang != LANG_NULL)
                name = cur_lang[option_item->lang];

            if (name && config_key & mapping_key)
            {
                if (n_maped > 0)
                    strcat(text, "+");
                strcat(text, name);
                n_maped++;
            }
        }
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

    CheckBoxOptionMenuItem *items = option_menu->items;
    int n_items = option_menu->n_items;

    int item_sx = option_listview_scroll_sx + OPTION_LISTVIEW_PADDING_L;
    int item_dx = option_listview_scroll_sx + option_itemview_width;
    int item_sy = option_listview_sy + OPTION_LISTVIEW_PADDING_T;
    int item_max_dy = option_listview_dy - OPTION_LISTVIEW_PADDING_T;
    int clip_height = option_itemview_height;
    int checkbox_width = GUI_GetFontSize();
    int checkbox_height = checkbox_width;
    int checkbox_sx = item_dx - OPTION_ITEMVIEW_PADDING_L - checkbox_width;
    int i;
    for (i = option_top_pos; i < n_items; i++)
    {
        if (item_sy >= item_max_dy)
            break;

        if (item_sy + option_itemview_height > item_max_dy)
            clip_height = item_max_dy - item_sy;
        else
            clip_height = option_itemview_height;

        GUI_EnableClipping();
        GUI_SetClipRectangle(item_sx, item_sy, option_itemview_width, clip_height);

        // Focus
        if (i == option_focus_pos)
            GUI_DrawFillRectangle(item_sx, item_sy, option_itemview_width, option_itemview_height, MENU_ITEMVIEW_COLOR_FOCUS_BG);

        if (items[i].name)
            GUI_DrawText(item_sx + OPTION_ITEMVIEW_PADDING_L, item_sy + OPTION_ITEMVIEW_PADDING_T, COLOR_GREEN, items[i].name);
        else if (items[i].lang != LANG_NULL)
            GUI_DrawText(item_sx + OPTION_ITEMVIEW_PADDING_L, item_sy + OPTION_ITEMVIEW_PADDING_T, COLOR_GREEN, cur_lang[items[i].lang]);

        GUI_DrawEmptyRectangle(checkbox_sx, item_sy + OPTION_ITEMVIEW_PADDING_T, checkbox_width, checkbox_height, 1.0f, COLOR_GREEN);
        if (items[i].selected)
            GUI_DrawFillRectangle(checkbox_sx + 1, item_sy + OPTION_ITEMVIEW_PADDING_T + 1, checkbox_width - 2, checkbox_height - 2, COLOR_ALPHA(COLOR_ORANGE, 0xBF));

        GUI_DisableClipping();
        item_sy += option_itemview_height;
    }

    int scrollbar_track_x = option_listview_scroll_sx + option_listview_width - GUI_DEF_SCROLLBAR_SIZE - 2;
    ;
    GUI_DrawVerticalScrollbar(scrollbar_track_x, option_scrollbar_track_y, option_scrollbar_track_height, n_items, option_listview_n_draw_items, option_top_pos, 0);
}

static void drawMenu()
{
    SettingMenuItem **items = select_menu_items;
    int n_items = n_select_menu_items;

    // Draw items
    if (items && n_items > 0)
    {
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

            if (item_sy + menu_itemview_height > item_max_dy)
                clip_height = item_max_dy - item_sy;
            else
                clip_height = menu_itemview_height;

            if (!*items[i]->visibility)
                continue;

            sy = item_sy + MENU_ITEMVIEW_PADDING_T;

            GUI_EnableClipping();
            GUI_SetClipRectangle(item_sx, item_sy, menu_itemview_width, clip_height);

            // Focus
            if (i == menu_focus_pos)
                GUI_DrawFillRectangle(item_sx, item_sy, menu_itemview_width, menu_itemview_height, MENU_ITEMVIEW_COLOR_FOCUS_BG);

            if (items[i]->name)
                GUI_DrawText(sx, sy, COLOR_WHITE, items[i]->name);
            else if (items[i]->lang != LANG_NULL)
                GUI_DrawText(sx, sy, COLOR_WHITE, cur_lang[items[i]->lang]);

            if (items[i]->option)
            {
                if (items[i]->option_type == TYPE_OPTION_STR_ARRAY) // Option string array
                {
                    StrArrayOption *option = (StrArrayOption *)items[i]->option;
                    int value = *(option->value);
                    if (option->names)
                        GUI_DrawText(dx - GUI_GetTextWidth(option->names[value]), sy, COLOR_GREEN, option->names[value]);
                    else if (option->langs)
                        GUI_DrawText(dx - GUI_GetTextWidth(cur_lang[option->langs[value]]), sy, COLOR_GREEN, cur_lang[option->langs[value]]);
                }
                else if (items[i]->option_type == TYPE_OPTION_INT_ARRAY) // Option int array
                {
                    IntArrayOption *option = (IntArrayOption *)items[i]->option;
                    int value = *(option->value);
                    const int key = option->values[value];
                    char name[256];
                    snprintf(name, 256, option->format, key);
                    GUI_DrawText(dx - GUI_GetTextWidth(name), sy, COLOR_GREEN, name);
                }
                else if (items[i]->option_type == TYPE_OPTION_INT_RANGE) // Option int range
                {
                    IntRangeOption *option = (IntRangeOption *)items[i]->option;
                    int value = *(option->value);
                    char name[256];
                    snprintf(name, 256, option->format, value);
                    GUI_DrawText(dx - GUI_GetTextWidth(name), sy, COLOR_GREEN, name);
                }
                else if (items[i]->option_type == TYPE_OPTION_CHECK_BOX) // Option check box
                {
                    CheckBoxOptionMenu *option = (CheckBoxOptionMenu *)items[i]->option;
                    int x = dx;
                    if (menu_list_focus_pos == INDEX_MENU_CONTROL)
                    {
                        int turbo = *(int *)(option->userdata) & TURBO_KEY_BITMASK;
                        uint32_t bg_color = turbo ? COLOR_ALPHA(COLOR_ORANGE, 0xBF) : COLOR_ALPHA(COLOR_BLACK, 0xBF);
                        uint32_t text_color = turbo ? COLOR_WHITE : COLOR_DARKGRAY;
                        char *turbo_str = cur_lang[LABEL_TURBO];
                        int turbo_text_width = GUI_GetTextWidth(turbo_str);
                        x = dx - turbo_text_width;
                        GUI_DrawFillRectangle(x, sy, turbo_text_width, GUI_GetFontSize(), bg_color);
                        GUI_DrawText(x, sy, text_color, turbo_str);
                    }
                    if (option->name)
                        GUI_DrawText(x - GUI_GetTextWidth(option->name) - 6, sy, COLOR_GREEN, option->name);
                    else if (option->lang != LANG_NULL)
                        GUI_DrawText(x - GUI_GetTextWidth(option->name) - 6, sy, COLOR_GREEN, cur_lang[option->lang]);
                }
            }

            GUI_DisableClipping();
            item_sy += menu_itemview_height;
        }

        GUI_DrawVerticalScrollbar(menu_scrollbar_track_x, menu_scrollbar_track_y, menu_scrollbar_track_height,
                                  n_items, menu_listview_n_draw_items, menu_top_pos, 0);

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
        if (!*menu_list[i]->visibility)
            continue;

        char *name = menu_list[i]->name;
        if (menu_list[i]->lang != LANG_NULL)
            name = cur_lang[menu_list[i]->lang];
        view_width = GUI_GetTextWidth(name) + MENU_TAB_VIEW_PADDING_L * 2;

        if (i == menu_list_focus_pos)
            GUI_DrawFillRectangle(view_sx, view_sy, view_width, menu_tab_view_height, MENU_ITEMVIEW_COLOR_FOCUS_BG);

        sx = view_sx + MENU_TAB_VIEW_PADDING_L;
        GUI_DrawText(sx, sy, COLOR_WHITE, name);
        view_sx += view_width;
    }

    // Battery
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
    GUI_DrawText(time_x, sy, GUI_DEF_COLOR_TEXT, time_string);

    GUI_DrawFillRectangle(menu_tab_view_sx, menu_tab_view_dy, menu_tab_view_width, MENU_TAB_LINEVIEW_HEIGHT, MENU_ITEMVIEW_COLOR_FOCUS_BG);
}

static void drawDialogCallback(GUI_Dialog *dialog)
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
            moveMenuListPos(TYPE_MOVE_NONE);
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
        moveMenuListPos(TYPE_MOVE_UP);
    }
    else if (released_pad[PAD_R1])
    {
        moveMenuListPos(TYPE_MOVE_DOWN);
    }
}

static void closeOption()
{
    option_open = 0;
}

static void ctrlOption()
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        MoveListPos(TYPE_MOVE_UP, &option_top_pos, &option_focus_pos, option_menu->n_items, option_listview_n_draw_items);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        MoveListPos(TYPE_MOVE_DOWN, &option_top_pos, &option_focus_pos, option_menu->n_items, option_listview_n_draw_items);
    }

    if (released_pad[PAD_ENTER] || released_pad[PAD_SQUARE])
    {
        option_menu->items[option_focus_pos].selected = !option_menu->items[option_focus_pos].selected;
        if (option_menu->updateCallback)
            option_menu->updateCallback(option_menu);
    }
    else if (released_pad[PAD_TRIANGLE])
    {
        int i;
        for (i = 0; i < option_menu->n_items; i++)
        {
            option_menu->items[i].selected = 0;
        }
        if (option_menu->updateCallback)
            option_menu->updateCallback(option_menu);
    }
    else if (released_pad[PAD_CANCEL])
    {
        closeOption();
    }
}

static void ctrlMenu()
{
    if (option_open)
    {
        ctrlOption();
        return;
    }

    SettingMenuItem *item = select_menu_items[menu_focus_pos];
    int n_items = n_select_menu_items;

    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        MoveListPos(TYPE_MOVE_UP, &menu_top_pos, &menu_focus_pos, n_items, menu_listview_n_draw_items);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        MoveListPos(TYPE_MOVE_DOWN, &menu_top_pos, &menu_focus_pos, n_items, menu_listview_n_draw_items);
    }

    if (released_pad[PAD_CANCEL])
    {
        GUI_CloseDialog(&setting_dialog);
    }
    else if (item->option)
    {
        if (item->option_type == TYPE_OPTION_CALLBACK)
        {
            void (*callback)() = (void (*)())item->option;
            if (released_pad[PAD_ENTER])
            {
                if (callback)
                    callback();
            }
        }
        else if (item->option_type == TYPE_OPTION_STR_ARRAY)
        {
            StrArrayOption *option = (StrArrayOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->n_names - 1)
                    (*(option->value))++;
                else
                    *(option->value) = 0;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > 0)
                    (*(option->value))--;
                else
                    *(option->value) = option->n_names - 1;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
        }
        else if (item->option_type == TYPE_OPTION_INT_ARRAY)
        {
            IntArrayOption *option = (IntArrayOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->n_values - 1)
                    (*(option->value))++;
                else
                    *(option->value) = 0;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > 0)
                    (*(option->value))--;
                else
                    *(option->value) = option->n_values - 1;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
        }
        else if (item->option_type == TYPE_OPTION_INT_RANGE)
        {
            IntRangeOption *option = (IntRangeOption *)item->option;
            if (released_pad[PAD_RIGHT] || released_pad[PAD_LEFT_ANALOG_RIGHT])
            {
                if (*(option->value) < option->max)
                    (*(option->value))++;
                else
                    *(option->value) = option->min;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
            else if (released_pad[PAD_LEFT] || released_pad[PAD_LEFT_ANALOG_LEFT])
            {
                if (*(option->value) > option->min)
                    (*(option->value))--;
                else
                    *(option->value) = option->max;
                if (option->updateCallback)
                    option->updateCallback(option);
            }
        }
        else if (item->option_type == TYPE_OPTION_CHECK_BOX)
        {
            CheckBoxOptionMenu *option = (CheckBoxOptionMenu *)item->option;
            if (released_pad[PAD_ENTER])
            {
                option_menu = option;
                option_top_pos = 0;
                option_focus_pos = 0;
                if (option->openCallback)
                    option->openCallback(option);
                option_open = 1;
            }
            else if (released_pad[PAD_SQUARE])
            {
                if (menu_list_focus_pos == INDEX_MENU_CONTROL)
                {
                    uint32_t *config_key = (uint32_t *)(option->userdata);
                    if (*config_key & TURBO_KEY_BITMASK)
                        *config_key &= ~TURBO_KEY_BITMASK;
                    else
                        *config_key |= TURBO_KEY_BITMASK;
                    control_option_changed = 1;
                }
            }
        }
    }
}

static void ctrlDialogCallback(GUI_Dialog *dialog)
{
    if (menu_items_need_update)
    {
        char *name = select_menu_items[menu_focus_pos]->name;
        if (select_menu_items[menu_focus_pos]->lang != LANG_NULL)
            name = cur_lang[select_menu_items[menu_focus_pos]->lang];
        refreshSelectMenuItems();
        if (name)
            setFocusByName(name);
        menu_items_need_update = 0;
    }

    ctrlCommon();

    if (menu_list_focus_pos == 1)
        Setting_CtrlState();
    else
        ctrlMenu();
}

static int openDialogCallback(GUI_Dialog *dialog)
{
    GUI_CloseOtherDialogs(dialog, TYPE_GUI_DIALOG_ANY);
    Setting_WaitOverlayInitEnd();

    current_path_is_file = CurrentPathIsFile();

    if (menu_need_update)
    {
        Setting_UpdateKeyMapperMenu(INDEX_MENU_CONTROL);
        Setting_UpdateKeyMapperMenu(INDEX_MENU_HOTKEY);

        game_is_loaded = Emu_IsGameLoaded();
        setting_config_type = game_is_loaded ? TYPE_CONFIG_GAME : TYPE_CONFIG_MAIN;
        app_menu_visibility = !game_is_loaded;
        core_menu_visibility = (menu_list[INDEX_MENU_CORE]->items && (game_is_loaded || has_main_core_menu));
        disk_control_visibility = (game_is_loaded && Emu_HasDiskControl() && Emu_DiskGetNumImages() > 0);
        exit_to_arch_visibility = (exec_boot_mode == BOOT_MODE_ARCH);
        touch_to_button_visibility = !is_vitatv_model;
    }

    if (menu_need_update || !*menu_list[menu_list_focus_pos]->visibility)
    {
        menu_list_focus_pos = 0;
        moveMenuListPos(TYPE_MOVE_NONE);
    }

    int i;
    for (i = 0; i < MENU_LIST_LEN; i++)
    {
        if (*menu_list[i]->visibility && menu_list[i]->enterCallback)
            menu_list[i]->enterCallback(menu_list[i]);
    }

    menu_need_update = 0;

    return 0;
}

static int closeDialogCallback(GUI_Dialog *dialog)
{
    int i;
    for (i = 0; i < MENU_LIST_LEN; i++)
    {
        if (*menu_list[i]->visibility && menu_list[i]->exitCallback)
            menu_list[i]->exitCallback(menu_list[i]);
    }

    if (game_is_loaded)
        Emu_ResumeGame();

    return 0;
}

int Setting_Init()
{
    if (core_menu.items)
        has_main_core_menu = 1;
    Setting_SetAppLangOption();
    Setting_InitOverlay();
    refreshSettingLayout();
    return 0;
}

int Setting_Deinit()
{
    return 0;
}