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

#define TEXT_COLOR COLOR_ORANGE

static Layout *about_layout = NULL;
static ListView *about_listView = NULL;

static int getListLength(void *list)
{
    if (!list)
        return 0;
    return N_ABOUT_TEXTS;
}

static void *getHeadListEntry(void *list)
{
    if (!list)
        return NULL;
    char **textList = (char **)list;
    return textList[0];
}

static void *getTailListEntry(void *list)
{
    if (!list)
        return NULL;
    char **textList = (char **)list;
    return textList[N_ABOUT_TEXTS - 1];
}

static void *getNextListEntry(void *list, void *cur_entry, int cur_idx)
{
    if (!list)
        return NULL;
    char **textList = (char **)list;
    if (cur_idx < N_ABOUT_TEXTS - 1)
        return textList[cur_idx + 1];
    return NULL;
}

static void *getPrevListEntry(void *list, void *cur_entry, int cur_idx)
{
    if (!list)
        return NULL;
    char **textList = (char **)list;
    if (cur_idx > 0)
        return textList[cur_idx - 1];
    return NULL;
}

static char *getName(void *list, void *entry, int idx)
{
    if (!list)
        return NULL;
    return (char *)entry;
}

static uint32_t getNameColor(void *list, void *entry, int idx)
{
    return TEXT_COLOR;
}

static void refreshLayout()
{
    int layout_w = 0, layout_h = 0;

    GUI_GetActivityLayoutWH(&about_activity, &layout_w, &layout_h);

    if (!about_layout)
    {
        about_layout = NewLayout();
        if (!about_layout)
            return;
        LayoutParamSetAutoFree(about_layout, 0);
    }
    LayoutSetOrientation(about_layout, TYPE_LAYOUT_ORIENTATION_VERTICAL);
    LayoutParamSetLayoutSize(about_layout, layout_w, layout_h);
    LayoutParamSetPadding(about_layout, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING);
    LayoutEmpty(about_layout);

    if (!about_listView)
    {
        about_listView = NewListView();
        if (!about_listView)
            return;
    }
    LayoutParamSetLayoutSize(about_listView, TYPE_LAYOUT_MATH_PARENT, TYPE_LAYOUT_MATH_PARENT);
    LayoutParamSetPadding(about_listView, LISTVIEW_PADDING_L, LISTVIEW_PADDING_L, LISTVIEW_PADDING_T, LISTVIEW_PADDING_T);
    ListViewSetBgColor(about_listView, GUI_DEF_COLOR_BG);
    ListViewSetItemPadding(about_listView, ITEMVIEW_PADDING_L, ITEMVIEW_PADDING_L, ITEMVIEW_PADDING_T, ITEMVIEW_PADDING_T);
    ListViewSetFocusPosEnabled(about_listView, 0);
    LayoutAdd(about_layout, about_listView);

    ListViewCallbacks callbacks;
    memset(&callbacks, 0, sizeof(ListViewCallbacks));
    callbacks.getListLength = getListLength;
    callbacks.getHeadListEntry = getHeadListEntry;
    callbacks.getTailListEntry = getTailListEntry;
    callbacks.getNextListEntry = getNextListEntry;
    callbacks.getPrevListEntry = getPrevListEntry;
    callbacks.getName = getName;
    callbacks.getNameColor = getNameColor;
    ListViewSetList(about_listView, about_texts, &callbacks);

    ViewRefresh(about_layout);
}

static int startActivityCallback(GUI_Activity *activity)
{
    about_activity.wallpaper = GUI_GetDefaultWallpaper();
    refreshLayout();
    return 0;
}

static void drawActivityCallback(GUI_Activity *activity)
{
    int layout_x = 0, layout_y = 0;
    GUI_GetActivityLayoutXY(&about_activity, &layout_x, &layout_y);
    LayoutDraw(about_layout, layout_x, layout_y);
}

static void ctrlActivityCallback(GUI_Activity *activity)
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        ListViewMovePos(about_listView, TYPE_MOVE_UP);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        ListViewMovePos(about_listView, TYPE_MOVE_DOWN);
    }
    else if (hold_pad[PAD_LEFT])
    {
        ListViewMovePos(about_listView, TYPE_MOVE_LEFT);
    }
    else if (hold_pad[PAD_RIGHT])
    {
        ListViewMovePos(about_listView, TYPE_MOVE_RIGHT);
    }

    if (released_pad[PAD_CANCEL])
    {
        GUI_ExitActivity(&about_activity);
    }
}
