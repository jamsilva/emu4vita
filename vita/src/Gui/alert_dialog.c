#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "List/text_list.h"
#include "alert_dialog.h"
#include "gui.h"
#include "utils.h"
#include "lang.h"

#define DIALOG_MAX_WIDTH (GUI_SCREEN_WIDTH * 4.f / 5.f)
#define DIALOG_MAX_HEIGHT (GUI_SCREEN_HEIGHT * 4.f / 5.f)
#define DIALOG_MIN_WIDTH (GUI_SCREEN_WIDTH / 2.f)
#define DIALOG_MIN_HEIGHT (GUI_SCREEN_HEIGHT / 2.f)

#define STATEBAR_PADDING_L 10
#define STATEBAR_PADDING_T 6

#define TIP_LISTVIEW_PADDING_L 6
#define TIP_LISTVIEW_PADDING_T 9
#define MENU_LISTVIEW_PADDING_L 6
#define MENU_LISTVIEW_PADDING_T 6

#define TIP_ITEMVIEW_PADDING_L 10
#define TIP_ITEMVIEW_PADDING_T 3
#define MENU_ITEMVIEW_PADDING_L 10
#define MENU_ITEMVIEW_PADDING_T 6

#define STATEBAR_HEIGHT (GUI_GetFontSize() + STATEBAR_PADDING_T * 2)
#define TIP_ITEMVIEW_HEIGHT (GUI_GetFontSize() + TIP_ITEMVIEW_PADDING_T * 2)
#define MENU_ITEMVIEW_HEIGHT (GUI_GetFontSize() + MENU_ITEMVIEW_PADDING_T * 2)

#define SCREEN_COLOR_VAGUE COLOR_ALPHA(COLOR_BLACK, 0x3F)
#define STATEBAR_COLOR_BG COLOR_ALPHA(0xFF473800, 0xFF)
#define DIALOG_COLOR_BG COLOR_ALPHA(0xFF2B2100, 0xFF)
#define ITEMVIEW_COLOR_FOCUS_BG COLOR_ALPHA(COLOR_ORANGE, 0xBF)
#define DIALOG_COLOR_TEXT COLOR_WHITE

#define MAX_COLOR_GRADUAL_COUNT 30
#define BEGIN_COLOR_GRADUAL_COUNT 0
#define STEP_COLOR_GRADUAL_COUNT 1

static void drawDialogCallback(GUI_Dialog *dialog);
static void ctrlDialogCallback(GUI_Dialog *dialog);
static int closeDialogCallback(GUI_Dialog *dialog);

int GetUTF8Count(const char *utf8)
{
    if ((utf8[0] & 0xF0) == 0xE0) // 0xF0 = 11110000, 0xE0 = 11100000
        return 3;
    else if ((utf8[0] & 0xE0) == 0xC0) // 0xE0 = 11100000, 0xC0= 11000000
        return 2;
    else
        return 1;
}

static int isEnglishCharacter(char ch)
{
    if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A))
        return 1;
    else
        return 0;
}

static uint32_t getGradualColor(uint32_t color, int gradual, int max)
{
    uint32_t rgb = color & 0x00FFFFFF;
    uint8_t a = color >> 24;
    a = a * ((float)gradual / (float)max);
    return (rgb | (a << 24));
}

static int convertStringToListByWidth(TextList *list, const char *str, int limit_width)
{
    if (!list || !str)
        return 0;

    int len = strlen(str);
    char *buf = malloc(len + 1);
    strcpy(buf, str);

    char *line = buf;
    char *space = buf;
    char *finish = buf + len;
    char *p = buf;
    int width = 0;
    int max_width = 0;
    int count;
    int ch_w;
    char ch;
    while (p < finish)
    {
        count = GetUTF8Count(p);
        ch = *(p + count);
        *(p + count) = '\0';
        ch_w = GUI_GetTextWidth(p);
        *(p + count) = ch;

        if (*p == ' ')
            space = p;

        if (*p == '\n' || width + ch_w > limit_width)
        {
            if (isEnglishCharacter(*p) && (p > line && isEnglishCharacter(*(p - 1))) && space > line)
            {
                // Go back to the last space
                p = space + 1;
                count = 0;
                ch_w = 0;
            }
            ch = *p;
            *p = '\0';
            TextListAddEntryEx(list, line);
            *p = ch;
            line = p;
            space = p;
            if (width > max_width)
                max_width = width;
            width = ch_w;
        }
        else
        {
            width += ch_w;
            if (width > max_width)
                max_width = width;
        }
        p += count;
    }
    if (line < finish)
        TextListAddEntryEx(list, line);

    free(buf);
    return max_width;
}

GUI_Dialog *AlertDialog_Creat()
{
    GUI_Dialog *dialog = malloc(sizeof(GUI_Dialog));
    if (!dialog)
        return NULL;
    memset(dialog, 0, sizeof(GUI_Dialog));

    AlertDialogData *data = malloc(sizeof(AlertDialogData));
    if (!data)
    {
        free(dialog);
        return NULL;
    }
    memset(data, 0, sizeof(AlertDialogData));
    data->auto_free = 1;
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;
    data->color_gradual = BEGIN_COLOR_GRADUAL_COUNT;
    data->dialog = dialog;
    dialog->userdata = data;
    dialog->drawCallback = drawDialogCallback;
    dialog->ctrlCallBack = ctrlDialogCallback;
    dialog->closeCallback = closeDialogCallback;

    return dialog;
}

void AlertDialog_Destroy(GUI_Dialog *dialog)
{
    if (!dialog || dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->freeCallback)
        data->freeCallback(dialog);
    if (data->title)
        free(data->title);
    if (data->positive_text)
        free(data->positive_text);
    if (data->negative_text)
        free(data->negative_text);
    TextListEmpty(&data->list);
    free(data);
    free(dialog);
}

void AlertDialog_SetAutoFree(GUI_Dialog *dialog, int auto_free)
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->auto_free = auto_free;
}

void AlertDialog_SetTitle(GUI_Dialog *dialog, char *title)
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->title)
        free(data->title);
    data->title = malloc(strlen(title) + 1);
    if (data->title)
        strcpy(data->title, title);
}

void AlertDialog_SetMessage(GUI_Dialog *dialog, char *message)
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->type = TYPE_DIALOG_TIP;
    TextListEmpty(&data->list);
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;

    if (!message)
        return;

    int limit_width = DIALOG_MAX_WIDTH - TIP_LISTVIEW_PADDING_L * 2 - TIP_ITEMVIEW_PADDING_L * 2;
    int max_width = convertStringToListByWidth(&data->list, message, limit_width);

    max_width += (TIP_LISTVIEW_PADDING_L * 2 + TIP_ITEMVIEW_PADDING_L * 2);
    if (max_width > DIALOG_MAX_WIDTH)
        data->dialog_width = DIALOG_MAX_WIDTH;
    else if (max_width < DIALOG_MIN_WIDTH)
        data->dialog_width = DIALOG_MIN_WIDTH;
    else
        data->dialog_width = max_width;

    int max_height = data->list.length * TIP_ITEMVIEW_HEIGHT + TIP_LISTVIEW_PADDING_T * 2 + STATEBAR_HEIGHT * 2;
    if (max_height > DIALOG_MAX_HEIGHT)
        data->dialog_height = DIALOG_MAX_HEIGHT;
    else if (max_height < DIALOG_MIN_HEIGHT)
        data->dialog_height = DIALOG_MIN_HEIGHT;
    else
        data->dialog_height = max_height;
}

void AlertDialog_SetMenu(GUI_Dialog *dialog, char **list, int list_len)
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->type = TYPE_DIALOG_MENU;
    TextListEmpty(&data->list);
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;

    if (!list)
        return;

    int limit_width = DIALOG_MAX_WIDTH - MENU_LISTVIEW_PADDING_L * 2 - MENU_ITEMVIEW_PADDING_L * 2;
    int max_width = 0;
    int i;
    for (i = 0; i < list_len; i++)
    {
        if (max_width < limit_width)
        {
            int w = GUI_GetTextWidth(list[i]);
            if (w > max_width)
                max_width = w;
        }
        TextListAddEntryEx(&data->list, list[i]);
    }

    max_width += (MENU_LISTVIEW_PADDING_L * 2 + MENU_ITEMVIEW_PADDING_L * 2);
    if (max_width > DIALOG_MAX_WIDTH)
        data->dialog_width = DIALOG_MAX_WIDTH;
    else if (max_width < DIALOG_MIN_WIDTH)
        data->dialog_width = DIALOG_MIN_WIDTH;
    else
        data->dialog_width = max_width;

    int max_height = data->list.length * MENU_ITEMVIEW_HEIGHT + MENU_LISTVIEW_PADDING_T * 2 + STATEBAR_HEIGHT * 2;
    if (max_height > DIALOG_MAX_HEIGHT)
        data->dialog_height = DIALOG_MAX_HEIGHT;
    else if (max_height < DIALOG_MIN_HEIGHT)
        data->dialog_height = DIALOG_MIN_HEIGHT;
    else
        data->dialog_height = max_height;
}

void AlertDialog_SetPositiveCallback(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->positive_text)
        free(data->positive_text);
    data->positive_text = malloc(strlen(name) + 1);
    strcpy(data->positive_text, name);
    data->positiveCallback = callback;
}

void AlertDialog_SetNegativeCallback(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->negative_text)
        free(data->negative_text);
    data->negative_text = malloc(strlen(name) + 1);
    strcpy(data->negative_text, name);
    data->negativeCallback = callback;
}

void AlertDialog_SetFreeCallback(GUI_Dialog *dialog, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->freeCallback = callback;
}

static void drawDialogCallback(GUI_Dialog *dialog)
{
    AlertDialogData *data = (AlertDialogData *)dialog->userdata;

    int dialog_w = data->dialog_width;
    int dialog_h = data->dialog_height;
    int dialog_x = (GUI_SCREEN_WIDTH - dialog_w) / 2;
    int dialog_y = (GUI_SCREEN_HEIGHT - dialog_h) / 2;

    int statebar_w = dialog_w;
    int statebar_h = STATEBAR_HEIGHT;

    int top_bar_x = dialog_x;
    int top_bar_y = dialog_y;

    int listview_w = dialog_w;
    int listview_h = dialog_h - statebar_h * 2;
    int listview_x = dialog_x;
    int listview_y = dialog_y + statebar_h;

    int bottom_bar_x = dialog_x;
    int bottom_bar_y = listview_y + listview_h;

    uint32_t screen_color = getGradualColor(SCREEN_COLOR_VAGUE, data->color_gradual, MAX_COLOR_GRADUAL_COUNT);
    uint32_t dialog_color = DIALOG_COLOR_BG;
    uint32_t statebar_color = STATEBAR_COLOR_BG;
    uint32_t text_color = DIALOG_COLOR_TEXT;
    uint32_t focus_color = ITEMVIEW_COLOR_FOCUS_BG;

    int x, y;
    int clip_w, clip_h;

    if (data->color_gradual < MAX_COLOR_GRADUAL_COUNT)
        data->color_gradual += STEP_COLOR_GRADUAL_COUNT;
    if (data->color_gradual > MAX_COLOR_GRADUAL_COUNT)
        data->color_gradual = MAX_COLOR_GRADUAL_COUNT;

    // Screen vague
    GUI_DrawFillRectangle(0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, screen_color);
    // Dialog bg
    GUI_DrawFillRectangle(dialog_x, dialog_y, dialog_w, dialog_h, dialog_color);
    // Top bar bg
    GUI_DrawFillRectangle(top_bar_x, top_bar_y, statebar_w, statebar_h, statebar_color);
    // Bottom bar bg
    GUI_DrawFillRectangle(bottom_bar_x, bottom_bar_y, statebar_w, statebar_h, statebar_color);

    // Draw title
    if (data->title)
    {
        x = top_bar_x + (statebar_w - GUI_GetTextWidth(data->title)) / 2;
        if (x < top_bar_x + STATEBAR_PADDING_L)
            x = top_bar_x + STATEBAR_PADDING_L;
        y = top_bar_y + STATEBAR_PADDING_T;
        clip_w = statebar_w - STATEBAR_PADDING_L * 2;
        clip_h = statebar_h;
        GUI_EnableClipping();
        GUI_SetClipRectangle(x, y, clip_w, clip_h);
        GUI_DrawText(x, y, text_color, data->title);
        GUI_DisableClipping();
    }

    // Draw list
    TextListEntry *entry = TextListGetEntryByNumber(&data->list, data->top_pos);
    if (entry)
    {
        int listview_padding_l, listview_padding_t;
        int itemview_padding_l, itemview_padding_t;
        int itemview_h;

        if (data->type == TYPE_DIALOG_MENU)
        {
            listview_padding_l = MENU_LISTVIEW_PADDING_L;
            listview_padding_t = MENU_LISTVIEW_PADDING_T;
            itemview_padding_l = MENU_ITEMVIEW_PADDING_L;
            itemview_padding_t = MENU_ITEMVIEW_PADDING_T;
            itemview_h = MENU_ITEMVIEW_HEIGHT;
        }
        else
        {
            listview_padding_l = TIP_LISTVIEW_PADDING_L;
            listview_padding_t = TIP_LISTVIEW_PADDING_T;
            itemview_padding_l = TIP_ITEMVIEW_PADDING_L;
            itemview_padding_t = TIP_ITEMVIEW_PADDING_T;
            itemview_h = TIP_ITEMVIEW_HEIGHT;
        }
        int max_itemview_dy = listview_y + listview_h - listview_padding_t;
        int itemview_x = listview_x + listview_padding_l;
        int itemview_y = listview_y + listview_padding_t;
        int itemview_w = listview_w - listview_padding_l * 2;

        int i;
        for (i = data->top_pos; i < data->list.length && entry; i++)
        {
            if (itemview_y >= max_itemview_dy)
                break;

            clip_w = itemview_w;
            clip_h = itemview_h;
            if (clip_h > max_itemview_dy - itemview_y)
                clip_h = max_itemview_dy - itemview_y;

            GUI_EnableClipping();

            GUI_SetClipRectangle(itemview_x, itemview_y, itemview_w, clip_h);
            if (data->type == TYPE_DIALOG_MENU && i == data->focus_pos)
                GUI_DrawFillRectangle(itemview_x, itemview_y, clip_w, clip_h, focus_color);

            x = itemview_x + itemview_padding_l;
            y = itemview_y + itemview_padding_t;
            clip_w = itemview_w - itemview_padding_l * 2;
            GUI_SetClipRectangle(x, itemview_y, clip_w, clip_h);
            GUI_DrawText(x, y, text_color, entry->text);

            GUI_DisableClipping();
            itemview_y += itemview_h;
            entry = entry->next;
        }
    }

    // Draw Button
    char buf[24];
    x = dialog_x + dialog_w - STATEBAR_PADDING_L;
    y = bottom_bar_y + STATEBAR_PADDING_T;
    if (data->positive_text)
    {
        snprintf(buf, 24, "%s:%s", GUI_GetStringByLangId(BUTTON_ENTER), data->positive_text);
        x -= GUI_GetTextWidth(buf);
        GUI_DrawText(x, y, text_color, buf);
        x -= STATEBAR_PADDING_L;
    }
    if (data->negative_text)
    {
        snprintf(buf, 24, "%s:%s", GUI_GetStringByLangId(BUTTON_CANCEL), data->negative_text);
        x -= GUI_GetTextWidth(buf);
        GUI_DrawText(x, y, text_color, buf);
    }
}

static void ctrlDialogCallback(GUI_Dialog *dialog)
{
    AlertDialogData *data = (AlertDialogData *)dialog->userdata;

    if (hold_pad[PAD_UP] || hold_pad[PAD_LEFT_ANALOG_UP])
    {
        if (data->type == TYPE_DIALOG_TIP)
        {
            int max_draw_len = (data->dialog_height - STATEBAR_HEIGHT * 2) / TIP_ITEMVIEW_HEIGHT;
            MoveListPosNoFocus(TYPE_MOVE_UP, &data->top_pos, data->list.length, max_draw_len);
        }
        else if (data->type == TYPE_DIALOG_MENU)
        {
            int max_draw_len = (data->dialog_height - STATEBAR_HEIGHT * 2) / MENU_ITEMVIEW_HEIGHT;
            MoveListPos(TYPE_MOVE_UP, &data->top_pos, &data->focus_pos, data->list.length, max_draw_len);
        }
    }
    else if (hold_pad[PAD_DOWN] || hold_pad[PAD_LEFT_ANALOG_DOWN])
    {
        if (data->type == TYPE_DIALOG_TIP)
        {
            int max_draw_len = (data->dialog_height - STATEBAR_HEIGHT * 2) / TIP_ITEMVIEW_HEIGHT;
            MoveListPosNoFocus(TYPE_MOVE_DOWN, &data->top_pos, data->list.length, max_draw_len);
        }
        else if (data->type == TYPE_DIALOG_MENU)
        {
            int max_draw_len = (data->dialog_height - STATEBAR_HEIGHT * 2) / MENU_ITEMVIEW_HEIGHT;
            MoveListPos(TYPE_MOVE_DOWN, &data->top_pos, &data->focus_pos, data->list.length, max_draw_len);
        }
    }

    if (released_pad[PAD_ENTER])
    {
        if (data->positiveCallback)
            data->positiveCallback(dialog);
        else
            GUI_CloseDialog(dialog);
    }
    else if (released_pad[PAD_CANCEL])
    {
        if (data->negativeCallback)
            data->negativeCallback(dialog);
        else
            GUI_CloseDialog(dialog);
    }
}

static int closeDialogCallback(GUI_Dialog *dialog)
{
    if (!dialog || !dialog->userdata)
        return -1;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->auto_free)
        AlertDialog_Destroy(dialog);

    return 0;
}
