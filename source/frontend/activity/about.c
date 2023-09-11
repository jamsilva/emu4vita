#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "utils.h"
#include "lang.h"

#include "about_text.c"

static int startActivityCallback(GUI_Activity *activity);
static void drawActivityCallback(GUI_Activity *activity);
static void ctrlActivityCallback(GUI_Activity *activity);

static GUI_ButtonInstruction button_instructions[] = {
    {BUTTON_CANCEL, BACK, 1},
    {LANG_NULL, LANG_NULL, 1},
};

GUI_Activity about_activity = {
    ABOUT_TITLE,           // Title
    button_instructions,   // Button instructions
    NULL,                  // Wallpaper
    startActivityCallback, // Start callback
    NULL,                  // Exit callback
    drawActivityCallback,  // Draw callback
    ctrlActivityCallback,  // Ctrl callback
    0,                     // Disable draw statusbar
    NULL,                  // Parent activity
    NULL,                  // User data
};

#define LISTVIEW_PADDING_L 4
#define LISTVIEW_PADDING_T 4
#define ITEMVIEW_PADDING_L 3
#define ITEMVIEW_PADDING_T 3
#define ITEMVIEW_HEIGHT (GUI_GetFontSize() + ITEMVIEW_PADDING_T * 2)

#define TEXT_COLOR COLOR_ORANGE

static int listview_top_pos = 0;

static int layout_x, layout_y;
static int layout_w, layout_h;

static int listview_x, listview_y;
static int listview_w, listview_h;
static int itemview_w, itemview_h;
static int listview_n_draw_items;

static int scrollbar_track_x, scrollbar_track_y;
static int scrollbar_track_height;

static void refreshLayout()
{
    GUI_GetActivityLayoutXY(&about_activity, &layout_x, &layout_y);
    GUI_GetActivityLayoutWH(&about_activity, &layout_w, &layout_h);

    listview_x = layout_x + GUI_DEF_MAIN_LAYOUT_PADDING;
    listview_y = layout_y + GUI_DEF_MAIN_LAYOUT_PADDING;
    listview_w = layout_w - GUI_DEF_MAIN_LAYOUT_PADDING * 2;
    listview_h = layout_h - GUI_DEF_MAIN_LAYOUT_PADDING * 2;

    itemview_w = listview_w - LISTVIEW_PADDING_L * 2;
    itemview_h = ITEMVIEW_HEIGHT;
    listview_n_draw_items = (listview_h - LISTVIEW_PADDING_T * 2) / itemview_h;

    scrollbar_track_x = listview_x + listview_w - GUI_DEF_SCROLLBAR_SIZE - 2;
    scrollbar_track_y = listview_y + 2;
    scrollbar_track_height = listview_h - 4;
}

static int startActivityCallback(GUI_Activity *activity)
{
    about_activity.wallpaper = GUI_GetDefaultWallpaper();
    refreshLayout();
    return 0;
}

static void drawActivityCallback(GUI_Activity *activity)
{
    // ListView bg
    GUI_DrawFillRectangle(listview_x, listview_y, listview_w, listview_h, GUI_DEF_COLOR_BG);

    // ItemView
    int itemview_x = listview_x + LISTVIEW_PADDING_L;
    int itemview_y = listview_y + LISTVIEW_PADDING_T;
    int itemview_max_dy = listview_y + listview_h - LISTVIEW_PADDING_T;
    int list_len = N_ABOUT_TEXTS;
    int x, y;
    int clip_w, clip_h;

    int i;
    for (i = listview_top_pos; i < list_len; i++)
    {
        if (itemview_y >= itemview_max_dy)
            break;

        x = itemview_x + ITEMVIEW_PADDING_L;
        y = itemview_y + ITEMVIEW_PADDING_T;
        clip_w = itemview_w - ITEMVIEW_PADDING_L * 2;
        clip_h = itemview_h;
        if (clip_h > itemview_max_dy - itemview_y)
            clip_h = itemview_max_dy - itemview_y;
        GUI_EnableClipping(x, itemview_y, clip_w, clip_h);
        GUI_DrawText(x, y, TEXT_COLOR, about_texts[i]);
        GUI_DisableClipping();
        itemview_y += itemview_h;
    }

    // Scrollbar
    GUI_DrawVerticalScrollbar(scrollbar_track_x, scrollbar_track_y, scrollbar_track_height, list_len, listview_n_draw_items, listview_top_pos, 0);
}

static void ctrlActivityCallback(GUI_Activity *activity)
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        MoveListPosNoFocus(TYPE_MOVE_UP, &listview_top_pos, N_ABOUT_TEXTS, listview_n_draw_items);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        MoveListPosNoFocus(TYPE_MOVE_DOWN, &listview_top_pos, N_ABOUT_TEXTS, listview_n_draw_items);
    }
    else if (hold_pad[PAD_LEFT])
    {
        MoveListPosNoFocus(TYPE_MOVE_LEFT, &listview_top_pos, N_ABOUT_TEXTS, listview_n_draw_items);
    }
    else if (hold_pad[PAD_RIGHT])
    {
        MoveListPosNoFocus(TYPE_MOVE_RIGHT, &listview_top_pos, N_ABOUT_TEXTS, listview_n_draw_items);
    }

    if (released_pad[PAD_CANCEL])
    {
        GUI_ExitActivity(&about_activity);
    }
}
