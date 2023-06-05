#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Gui/gui.h"
#include "about_text.h"
#include "utils.h"
#include "lang.h"

static int ActivityInit();
static void ActivityDraw();
static void ActivityCtrl();

static GUI_ButtonInstruction button_instructions[] = {
    {STR_BUTTON_CANCEL, STR_BACK_TO_BROWSER},
    {NULL, NULL},
};

GUI_Activity about_activity = {
    STR_ABOUT,           // Title
    button_instructions, // Button instructions
    ActivityInit,        // Init callback
    NULL,                // Exit callback
    ActivityDraw,        // Draw callback
    ActivityCtrl,        // Ctrl callback
    0,                   // Disable draw statusbar
    0,                   // Disable draw wallpaper
    NULL,                // Parent activity
};

#define LISTVIEW_PADDING_L 4.0f
#define LISTVIEW_PADDING_T 4.0f
#define ITEMVIEW_PADDING_L 3.0f
#define ITEMVIEW_PADDING_T 3.0f

#define TEXT_COLOR COLOR_ORANGE

static int listview_width, listview_height;
static int listview_sx, listview_sy, listview_dx, listview_dy;
static int itemview_width, itemview_height;
static int list_max_draw_len;
static int scrollbar_track_x, scrollbar_track_y, scrollbar_track_height;

static int list_top_pos = 0;

static void refreshLayout()
{
    listview_width = ACTIVITY_FREE_DRAW_WIDTH;
    listview_height = ACTIVITY_FREE_DRAW_HEIGHT;
    listview_sx = ACTIVITY_FREE_DRAW_SX;
    listview_sy = ACTIVITY_FREE_DRAW_SY;
    listview_dx = ACTIVITY_FREE_DRAW_DX;
    listview_dy = ACTIVITY_FREE_DRAW_DY;

    itemview_width = listview_width - LISTVIEW_PADDING_L * 2;
    itemview_height = GUI_GetFontSize() + ITEMVIEW_PADDING_T * 2;
    list_max_draw_len = (listview_height - LISTVIEW_PADDING_T * 2) / itemview_height;

    scrollbar_track_x = listview_dx - GUI_SCROLLBAR_SIZE;
    scrollbar_track_y = listview_sy;
    scrollbar_track_height = listview_height;
}

static int ActivityInit()
{
    refreshLayout();
    return 0;
}

static void ActivityDraw()
{
    // Listview bg
    GUI_DrawFillRectangle(listview_sx, listview_sy, listview_width, listview_height, GUI_DEFALUT_BG_COLOR);

    // List
    int itemview_sx = listview_sx + LISTVIEW_PADDING_L;
    int itemview_sy = listview_sy + LISTVIEW_PADDING_T;
    int sx = itemview_sx + ITEMVIEW_PADDING_L;
    int sy;
    int drawn_len = 0;

    int i;
    for (i = list_top_pos; i < N_ABOUT_TEXTS; i++)
    {
        if (drawn_len >= list_max_draw_len)
            break;

        sy = itemview_sy + ITEMVIEW_PADDING_T;
        GUI_DrawText(sx, sy, TEXT_COLOR, about_texts[i]);
        itemview_sy += itemview_height;
        drawn_len++;
    }

    // Scrollbar
    GUI_DrawVerticalScrollbar(scrollbar_track_x, scrollbar_track_y, scrollbar_track_height, N_ABOUT_TEXTS, list_max_draw_len, list_top_pos, 0);
}

static void ActivityCtrl()
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        MoveListPosNoFocus(MOVE_TYPE_UP, &list_top_pos, N_ABOUT_TEXTS, list_max_draw_len);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        MoveListPosNoFocus(MOVE_TYPE_DOWN, &list_top_pos, N_ABOUT_TEXTS, list_max_draw_len);
    }
    else if (hold_pad[PAD_LEFT])
    {
        MoveListPosNoFocus(MOVE_TYPE_LEFT, &list_top_pos, N_ABOUT_TEXTS, list_max_draw_len);
    }
    else if (hold_pad[PAD_RIGHT])
    {
        MoveListPosNoFocus(MOVE_TYPE_RIGHT, &list_top_pos, N_ABOUT_TEXTS, list_max_draw_len);
    }

    if (pressed_pad[PAD_CANCEL])
    {
        GUI_ExitActivity(&about_activity);
    }
}