#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "list/string_list.h"
#include "gui/gui.h"
#include "alert_dialog.h"
#include "utils.h"
#include "lang.h"

#define DIALOG_MAX_WIDTH (GUI_SCREEN_WIDTH * 4.f / 5.f)
#define DIALOG_MAX_HEIGHT (GUI_SCREEN_HEIGHT * 4.f / 5.f)
#define DIALOG_MIN_WIDTH (GUI_SCREEN_WIDTH / 2.f)
#define DIALOG_MIN_HEIGHT (GUI_SCREEN_HEIGHT / 2.f)

#define STATEBAR_PADDING_L 10
#define STATEBAR_PADDING_T 6

#define TIP_LISTVIEW_PADDING_L 6
#define TIP_LISTVIEW_PADDING_T 13
#define TIP_ITEMVIEW_PADDING_L 10
#define TIP_ITEMVIEW_PADDING_T 3

#define MENU_LISTVIEW_PADDING_L 6
#define MENU_LISTVIEW_PADDING_T 10
#define MENU_ITEMVIEW_PADDING_L 10
#define MENU_ITEMVIEW_PADDING_T 6

#define STATEBAR_HEIGHT (GUI_GetFontSize() + STATEBAR_PADDING_T * 2)
#define TIP_ITEMVIEW_HEIGHT (GUI_GetFontSize() + TIP_ITEMVIEW_PADDING_T * 2)
#define MENU_ITEMVIEW_HEIGHT (GUI_GetFontSize() + MENU_ITEMVIEW_PADDING_T * 2)

#define OVERLAY_COLOR COLOR_ALPHA(COLOR_BLACK, 0x5F)
#define STATEBAR_COLOR_BG 0xFF7E5719
#define DIALOG_COLOR_BG 0xFF6F480A
#define ITEMVIEW_COLOR_FOCUS_BG COLOR_ALPHA(COLOR_ORANGE, 0xDF)
#define DIALOG_COLOR_TEXT COLOR_WHITE

#define MAX_DIALOG_GRADUAL_COUNT 10

static void drawDialogCallback(GUI_Dialog *dialog);
static void ctrlDialogCallback(GUI_Dialog *dialog);
static int openDialogCallback(GUI_Dialog *dialog);
static int closeDialogCallback(GUI_Dialog *dialog);

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

static int convertStringToListByWidth(LinkedList *list, const char *str, int limit_width)
{
    if (!list || !str)
        return 0;

    int len = strlen(str);
    char *buf = malloc(len + 1);
    if (!buf)
        return 0;
    strcpy(buf, str);

    char *string;
    char *start = buf;
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
            // Check english word truncated (if current character and space is not in the line's first)
            if ((p > start && space > start) && (isEnglishCharacter(*p) && isEnglishCharacter(*(p - 1))))
            {
                // Go back to the last space, current word will be in the next line
                p = space + 1;
                count = 0; // Set to zero for skip auto step
                ch_w = 0;  // Set to zero for skip auto step
            }
            ch = *p;
            *p = '\0';
            string = malloc(strlen(start) + 1);
            if (string)
            {
                strcpy(string, start);
                LinkedListAdd(list, string);
            }
            *p = ch;
            if (*p == '\n')
                p++;
            start = p;
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
    if (start < finish)
    {
        string = malloc(strlen(start) + 1);
        if (string)
        {
            strcpy(string, start);
            LinkedListAdd(list, string);
        }
    }

    free(buf);
    return max_width;
}

GUI_Dialog *AlertDialog_Create()
{
    GUI_Dialog *dialog = malloc(sizeof(GUI_Dialog));
    if (!dialog)
        return NULL;

    memset(dialog, 0, sizeof(GUI_Dialog));
    dialog->type = TYPE_GUI_DIALOG_ALERT;
    dialog->drawCallback = drawDialogCallback;
    dialog->ctrlCallBack = ctrlDialogCallback;
    dialog->openCallback = openDialogCallback;
    dialog->closeCallback = closeDialogCallback;

    AlertDialogData *data = malloc(sizeof(AlertDialogData));
    if (!data)
        goto FAILED;
    dialog->userdata = data;

    memset(data, 0, sizeof(AlertDialogData));
    data->auto_free = 1;
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;
    data->dialog = dialog;
    data->list = NewStringList();
    if (!data->list)
        goto FAILED;

    return dialog;

FAILED:
    if (dialog)
        AlertDialog_Destroy(dialog);
    return NULL;
}

void AlertDialog_Destroy(GUI_Dialog *dialog)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT)
        return;

    if (dialog->userdata)
    {
        AlertDialogData *data = (AlertDialogData *)dialog->userdata;
        if (data->freeCallback)
            data->freeCallback(dialog);
        if (data->title)
            free(data->title);
        if (data->positive_text)
            free(data->positive_text);
        if (data->negative_text)
            free(data->negative_text);
        if (data->neutral_text)
            free(data->neutral_text);
        if (data->list)
            LinkedListDestroy(data->list);
        free(data);
    }

    free(dialog);
}

void AlertDialog_SetAutoFree(GUI_Dialog *dialog, int auto_free)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->auto_free = auto_free;
}

void AlertDialog_SetUserdata(GUI_Dialog *dialog, void *userdata)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->userdata = userdata;
}

void AlertDialog_SetTitle(GUI_Dialog *dialog, char *title)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
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
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->type = TYPE_ALERT_DIALOG_TIP;
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;
    LinkedListEmpty(data->list);

    if (!message)
        return;

    int item_height = TIP_ITEMVIEW_HEIGHT;
    int limit_width = DIALOG_MAX_WIDTH - TIP_LISTVIEW_PADDING_L * 2 - TIP_ITEMVIEW_PADDING_L * 2;
    int max_width = convertStringToListByWidth(data->list, message, limit_width);

    max_width += (TIP_LISTVIEW_PADDING_L * 2 + TIP_ITEMVIEW_PADDING_L * 2);
    if (max_width > DIALOG_MAX_WIDTH)
        data->dialog_width = DIALOG_MAX_WIDTH;
    else if (max_width < DIALOG_MIN_WIDTH)
        data->dialog_width = DIALOG_MIN_WIDTH;
    else
        data->dialog_width = max_width;

    int l_length = LinkedListGetLength(data->list);
    int max_height = l_length * item_height + TIP_LISTVIEW_PADDING_T * 2 + STATEBAR_HEIGHT * 2;
    if (max_height > DIALOG_MAX_HEIGHT)
        data->dialog_height = DIALOG_MAX_HEIGHT;
    else if (max_height < DIALOG_MIN_HEIGHT)
        data->dialog_height = DIALOG_MIN_HEIGHT;
    else
        data->dialog_height = max_height;

    int listview_h = data->dialog_height - STATEBAR_HEIGHT * 2;
    data->listview_n_draw_items = (listview_h - TIP_LISTVIEW_PADDING_T * 2) / item_height;
}

void AlertDialog_SetItems(GUI_Dialog *dialog, char **items, int n_items)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->type = TYPE_ALERT_DIALOG_MENU;
    data->dialog_width = DIALOG_MIN_WIDTH;
    data->dialog_height = DIALOG_MIN_HEIGHT;
    LinkedListEmpty(data->list);

    if (!items)
        return;

    char *string;
    int item_height = TIP_ITEMVIEW_HEIGHT;
    int limit_width = DIALOG_MAX_WIDTH - MENU_LISTVIEW_PADDING_L * 2 - MENU_ITEMVIEW_PADDING_L * 2;
    int max_width = 0;
    int i;
    for (i = 0; i < n_items; i++)
    {
        if (max_width < limit_width)
        {
            int w = GUI_GetTextWidth(items[i]);
            if (w > max_width)
                max_width = w;
        }
        string = malloc(strlen(items[i]) + 1);
        if (string)
        {
            strcpy(string, items[i]);
            LinkedListAdd(data->list, string);
        }
    }

    max_width += (MENU_LISTVIEW_PADDING_L * 2 + MENU_ITEMVIEW_PADDING_L * 2);
    if (max_width > DIALOG_MAX_WIDTH)
        data->dialog_width = DIALOG_MAX_WIDTH;
    else if (max_width < DIALOG_MIN_WIDTH)
        data->dialog_width = DIALOG_MIN_WIDTH;
    else
        data->dialog_width = max_width;

    int l_length = LinkedListGetLength(data->list);
    int max_height = l_length * item_height + MENU_LISTVIEW_PADDING_T * 2 + STATEBAR_HEIGHT * 2;
    if (max_height > DIALOG_MAX_HEIGHT)
        data->dialog_height = DIALOG_MAX_HEIGHT;
    else if (max_height < DIALOG_MIN_HEIGHT)
        data->dialog_height = DIALOG_MIN_HEIGHT;
    else
        data->dialog_height = max_height;

    int listview_h = data->dialog_height - STATEBAR_HEIGHT * 2;
    data->listview_n_draw_items = (listview_h - TIP_LISTVIEW_PADDING_T * 2) / item_height;
}

void AlertDialog_SetPositiveButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->positive_text)
        free(data->positive_text);
    if (name)
    {
        data->positive_text = malloc(strlen(name) + 1);
        strcpy(data->positive_text, name);
    }
    data->positiveCallback = callback;
}

void AlertDialog_SetNegativeButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->negative_text)
        free(data->negative_text);
    if (name)
    {
        data->negative_text = malloc(strlen(name) + 1);
        strcpy(data->negative_text, name);
    }
    data->negativeCallback = callback;
}

void AlertDialog_setNeutralButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (data->neutral_text)
        free(data->neutral_text);
    if (name)
    {
        data->neutral_text = malloc(strlen(name) + 1);
        strcpy(data->neutral_text, name);
    }
    data->neutralCallback = callback;
}

void AlertDialog_SetFreeCallback(GUI_Dialog *dialog, void (*callback)(GUI_Dialog *dialog))
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
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

    uint32_t overlay_color = getGradualColor(OVERLAY_COLOR, data->gradual_count, MAX_DIALOG_GRADUAL_COUNT);
    uint32_t dialog_color = DIALOG_COLOR_BG;
    uint32_t statebar_color = STATEBAR_COLOR_BG;
    uint32_t text_color = DIALOG_COLOR_TEXT;
    uint32_t focus_color = ITEMVIEW_COLOR_FOCUS_BG;

    int x, y;
    int clip_w, clip_h;

    // Draw overlay
    GUI_DrawFillRectangle(0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, overlay_color);

    if (data->status == TYPE_DIALOG_STATUS_SHOW)
    {
        if (data->gradual_count < MAX_DIALOG_GRADUAL_COUNT)
        {
            data->gradual_count++;
            return;
        }
    }
    else
    {
        if (data->gradual_count > 0)
            data->gradual_count--;
        else
            GUI_CloseDialog(dialog);
        return;
    }

    // Draw dialog bg
    GUI_DrawFillRectangle(dialog_x, dialog_y, dialog_w, dialog_h, dialog_color);
    // Draw top bar bg
    GUI_DrawFillRectangle(top_bar_x, top_bar_y, statebar_w, statebar_h, statebar_color);
    // Draw bottom bar bg
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
        GUI_EnableClipping(x, top_bar_y, clip_w, clip_h);
        GUI_DrawText(x, y, text_color, data->title);
        GUI_DisableClipping();
    }

    // Draw list
    LinkedListEntry *entry = LinkedListFindByNum(data->list, data->top_pos);
    if (entry)
    {
        int listview_padding_l, listview_padding_t;
        int itemview_padding_l, itemview_padding_t;
        int itemview_h;

        if (data->type == TYPE_ALERT_DIALOG_MENU)
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
        int l_length = LinkedListGetLength(data->list);
        const char *text;

        int i;
        for (i = data->top_pos; i < l_length && entry; i++)
        {
            if (itemview_y >= max_itemview_dy)
                break;

            text = (char *)LinkedListGetEntryData(entry);
            if (!text)
                continue;

            clip_w = itemview_w;
            clip_h = itemview_h;
            if (clip_h > max_itemview_dy - itemview_y)
                clip_h = max_itemview_dy - itemview_y;

            GUI_EnableClipping(itemview_x, itemview_y, clip_w, clip_h);
            if (data->type == TYPE_ALERT_DIALOG_MENU && i == data->focus_pos)
                GUI_DrawFillRectangle(itemview_x, itemview_y, clip_w, clip_h, focus_color);
            GUI_DisableClipping();
            
            x = itemview_x + itemview_padding_l;
            y = itemview_y + itemview_padding_t;
            clip_w = itemview_w - itemview_padding_l * 2;
            GUI_EnableClipping(x, itemview_y, clip_w, clip_h);
            GUI_DrawText(x, y, text_color, text);
            GUI_DisableClipping();

            itemview_y += itemview_h;
            entry = LinkedListNext(entry);
        }

        // Draw scrollbar
        int track_x = listview_x + listview_w - GUI_DEF_SCROLLBAR_SIZE - 2;
        int track_y = listview_y + 2;
        int track_h = listview_h - 4;
        GUI_DrawVerticalScrollbar(track_x, track_y, track_h, l_length, data->listview_n_draw_items, data->top_pos, 0);
    }

    // Draw button
    char buf[24];
    x = dialog_x + dialog_w - STATEBAR_PADDING_L;
    y = bottom_bar_y + STATEBAR_PADDING_T;
    if (data->positive_text)
    {
        snprintf(buf, 24, "%s:%s", cur_lang[BUTTON_ENTER], data->positive_text);
        x -= GUI_GetTextWidth(buf);
        GUI_DrawText(x, y, text_color, buf);
        x -= STATEBAR_PADDING_L;
    }
    if (data->neutral_text)
    {
        snprintf(buf, 24, "%s:%s", cur_lang[BUTTON_TRIANGLE], data->neutral_text);
        x -= GUI_GetTextWidth(buf);
        GUI_DrawText(x, y, text_color, buf);
        x -= STATEBAR_PADDING_L;
    }
    if (data->negative_text)
    {
        snprintf(buf, 24, "%s:%s", cur_lang[BUTTON_CANCEL], data->negative_text);
        x -= GUI_GetTextWidth(buf);
        GUI_DrawText(x, y, text_color, buf);
    }
}

static void ctrlDialogCallback(GUI_Dialog *dialog)
{
    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    int l_length = LinkedListGetLength(data->list);

    if (hold_pad[PAD_UP] || hold_pad[PAD_LEFT_ANALOG_UP])
    {
        if (data->type == TYPE_ALERT_DIALOG_MENU)
            MoveListPos(TYPE_MOVE_UP, &data->top_pos, &data->focus_pos, l_length, data->listview_n_draw_items);
        else
            MoveListPosNoFocus(TYPE_MOVE_UP, &data->top_pos, l_length, data->listview_n_draw_items);
    }
    else if (hold_pad[PAD_DOWN] || hold_pad[PAD_LEFT_ANALOG_DOWN])
    {
        if (data->type == TYPE_ALERT_DIALOG_MENU)
            MoveListPos(TYPE_MOVE_DOWN, &data->top_pos, &data->focus_pos, l_length, data->listview_n_draw_items);
        else
            MoveListPosNoFocus(TYPE_MOVE_DOWN, &data->top_pos, l_length, data->listview_n_draw_items);
    }

    if (released_pad[PAD_ENTER])
    {
        if (data->positiveCallback)
            data->positiveCallback(dialog);
    }
    else if (released_pad[PAD_CANCEL])
    {
        if (data->negativeCallback)
            data->negativeCallback(dialog);
        else
            AlertDialog_Dismiss(dialog);
    }
    else if (released_pad[PAD_TRIANGLE])
    {
        if (data->neutralCallback)
            data->neutralCallback(dialog);
    }
}

static int openDialogCallback(GUI_Dialog *dialog)
{
    if (!dialog || !dialog->userdata)
        return -1;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->opened = 1;
    data->status = TYPE_DIALOG_STATUS_SHOW;
    data->gradual_count = 0;

    return 0;
}

static int closeDialogCallback(GUI_Dialog *dialog)
{
    if (!dialog || !dialog->userdata)
        return -1;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->opened = 0;
    data->status = TYPE_DIALOG_STATUS_DISMISS;
    if (data->auto_free)
        AlertDialog_Destroy(dialog);

    return 0;
}

int AlertDialog_ShowSimpleTipDialog(char *title, char *message)
{
    GUI_Dialog *tip_dialog = AlertDialog_Create();
    if (!tip_dialog)
        return -1;
    AlertDialog_SetTitle(tip_dialog, title);
    AlertDialog_SetMessage(tip_dialog, message);
    AlertDialog_SetNegativeButton(tip_dialog, cur_lang[COLSE], NULL);
    AlertDialog_Show(tip_dialog);
    return 0;
}

int AlertDialog_Show(GUI_Dialog *dialog)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return -1;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    if (!data->opened)
        GUI_OpenDialog(dialog);
    else
        data->status = TYPE_DIALOG_STATUS_SHOW;

    return 0;
}

int AlertDialog_Dismiss(GUI_Dialog *dialog)
{
    if (!dialog || dialog->type != TYPE_GUI_DIALOG_ALERT || !dialog->userdata)
        return -1;

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    data->status = TYPE_DIALOG_STATUS_DISMISS;

    return 0;
}
