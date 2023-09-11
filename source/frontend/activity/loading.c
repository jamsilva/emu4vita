#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/kernel/threadmgr.h>

#include "list/string_list.h"
#include "gui/gui.h"
#include "loading.h"
#include "utils.h"
#include "config.h"
#include "lang.h"

static int startActivityCallback(GUI_Activity *activity);
static int exitActivityCallback(GUI_Activity *activity);
static void drawActivityCallback(GUI_Activity *activity);
static void ctrlActivityCallback(GUI_Activity *activity);

GUI_Activity loading_activity = {
    LANG_NULL,             // Title
    NULL,                  // Button instructions
    NULL,                  // Wallpaper
    startActivityCallback, // Start callback
    exitActivityCallback,  // Exit callback
    drawActivityCallback,  // Draw callback
    ctrlActivityCallback,  // Ctrl callback
    1,                     // Disable draw statusbar
    NULL,                  // Parent activity
    NULL,                  // User data
};

#define LISTVIEW_PADDING_L 10
#define LISTVIEW_PADDING_T 10

#define ITEMVIEW_PADDING_L 4
#define ITEMVIEW_PADDING_T 4
#define ITEMVIEW_HEIGHT (GUI_GetFontSize() + ITEMVIEW_PADDING_T * 2)

#define LISTVIEW_COLOR_BG COLOR_ALPHA(COLOR_BLACK, 0xCF)
#define TEXT_COLOR COLOR_WHITE

#define LISTVIEW_PADDING_L 10
#define LISTVIEW_PADDING_T 10

#define ITEMVIEW_PADDING_L 4
#define ITEMVIEW_PADDING_T 4
#define ITEMVIEW_HEIGHT (GUI_GetFontSize() + ITEMVIEW_PADDING_T * 2)

#define LISTVIEW_COLOR_BG COLOR_ALPHA(COLOR_BLACK, 0xCF)
#define TEXT_COLOR COLOR_WHITE

static SceUID loading_thid = -1;
static int loading_thread_run = 0;
static SceKernelLwMutexWork laoding_mutex;
static LinkedList *log_list = NULL;

static int listview_auto_scroll = 0;
static int listview_n_draw_items = 0;
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
    GUI_GetActivityLayoutXY(&loading_activity, &layout_x, &layout_y);
    GUI_GetActivityLayoutWH(&loading_activity, &layout_w, &layout_h);

    listview_x = layout_x;
    listview_y = layout_y;
    listview_w = layout_w;
    listview_h = layout_h;

    itemview_w = listview_w - LISTVIEW_PADDING_L * 2;
    itemview_h = ITEMVIEW_HEIGHT;
    listview_n_draw_items = (listview_h - LISTVIEW_PADDING_T * 2) / itemview_h;

    scrollbar_track_x = listview_x + listview_w - GUI_DEF_SCROLLBAR_SIZE - 2;
    scrollbar_track_y = listview_y + 2;
    scrollbar_track_height = listview_h - 4;
}

void Loading_AddLog(const char *text)
{
    if (!text)
        return;

    sceKernelLockLwMutex(&laoding_mutex, 1, NULL);

    char *string = malloc(strlen(text) + 1);
    if (string)
    {
        strcpy(string, text);
        LinkedListAdd(log_list, string);
    }

    sceKernelUnlockLwMutex(&laoding_mutex, 1);
}

void Loading_AddLogf(const char *text, ...)
{
    char buf[1024];
    va_list argptr;
    va_start(argptr, (void *)text);
    vsnprintf(buf, sizeof(buf), text, argptr);
    va_end(argptr);

    Loading_AddLog(buf);
}

void Loading_SetAutoScrollListview(int enable)
{
    listview_auto_scroll = enable;
}

static void drawActivityCallback(GUI_Activity *activity)
{
    if (!app_config.show_log || !log_list)
        return;

    // Listview bg
    GUI_DrawFillRectangle(listview_x, listview_y, listview_w, listview_h, LISTVIEW_COLOR_BG);

    sceKernelLockLwMutex(&laoding_mutex, 1, NULL);

    int l_length = LinkedListGetLength(log_list);
    LinkedListEntry *entry = LinkedListFind(log_list, listview_top_pos);

    if (entry)
    {
        // Itemview
        int itemview_x = listview_x + LISTVIEW_PADDING_L;
        int itemview_y = listview_y + LISTVIEW_PADDING_T;
        int itemview_max_dy = listview_y + listview_h - LISTVIEW_PADDING_T;
        int x, y;
        int clip_w, clip_h;
        const char *text;

        int i;
        for (i = listview_top_pos; i < l_length; i++)
        {
            if (itemview_y >= itemview_max_dy)
                break;

            text = (char *)LinkedListGetEntryData(entry);
            if (!text)
                continue;

            x = itemview_x + ITEMVIEW_PADDING_L;
            y = itemview_y + ITEMVIEW_PADDING_T;
            clip_w = itemview_w - ITEMVIEW_PADDING_L * 2;
            clip_h = itemview_h;
            if (clip_h > itemview_max_dy - itemview_y)
                clip_h = itemview_max_dy - itemview_y;
            GUI_EnableClipping(x, itemview_y, clip_w, clip_h);
            GUI_DrawText(x, y, TEXT_COLOR, text);
            GUI_DisableClipping();
            itemview_y += itemview_h;
            entry = LinkedListNext(entry);
        }

        // Scrollbar
        GUI_DrawVerticalScrollbar(scrollbar_track_x, scrollbar_track_y, scrollbar_track_height, l_length, listview_n_draw_items, listview_top_pos, 0);
    }

    sceKernelUnlockLwMutex(&laoding_mutex, 1);
}

static void ctrlActivityCallback(GUI_Activity *activity)
{
    if (!app_config.show_log || !log_list)
        return;

    sceKernelLockLwMutex(&laoding_mutex, 1, NULL);

    int l_length = LinkedListGetLength(log_list);

    if (listview_auto_scroll)
    {
        listview_top_pos = l_length;
        RefreshListPosNoFocus(&listview_top_pos, l_length, listview_n_draw_items);
    }
    else
    {
        if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
        {
            MoveListPosNoFocus(TYPE_MOVE_UP, &listview_top_pos, l_length, listview_n_draw_items);
        }
        else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
        {
            MoveListPosNoFocus(TYPE_MOVE_DOWN, &listview_top_pos, l_length, listview_n_draw_items);
        }
        else if (hold_pad[PAD_LEFT])
        {
            MoveListPosNoFocus(TYPE_MOVE_LEFT, &listview_top_pos, l_length, listview_n_draw_items);
        }
        else if (hold_pad[PAD_RIGHT])
        {
            MoveListPosNoFocus(TYPE_MOVE_RIGHT, &listview_top_pos, l_length, listview_n_draw_items);
        }

        if (released_pad[PAD_CANCEL])
        {
            GUI_ExitActivity(&loading_activity);
        }
    }

    sceKernelUnlockLwMutex(&laoding_mutex, 1);
}

static int startActivityCallback(GUI_Activity *activity)
{
    loading_activity.wallpaper = GUI_GetDefaultSplash();
    refreshLayout();

    if (log_list)
        LinkedListDestroy(log_list);
    log_list = StringListCreate();

    sceKernelCreateLwMutex(&laoding_mutex, "laoding_mutex", 2, 0, NULL);
    return 0;
}

static int exitActivityCallback(GUI_Activity *activity)
{
    loading_thread_run = 0;
    sceKernelLockLwMutex(&laoding_mutex, 1, NULL);
    if (log_list)
        LinkedListDestroy(log_list);
    log_list = NULL;
    sceKernelUnlockLwMutex(&laoding_mutex, 1);
    sceKernelDeleteLwMutex(&laoding_mutex);
    return 0;
}

static int loadingThreadFunc(SceSize args, void *argp)
{
    GUI_StartActivity(&loading_activity);

    while (loading_thread_run)
    {
        GUI_RunMain();
    }

    GUI_ExitActivity(&loading_activity);

    sceKernelExitDeleteThread(0);
    return 0;
}

void Loading_WaitActivityThreadEnd()
{
    if (loading_thid >= 0)
    {
        sceKernelWaitThreadEnd(loading_thid, NULL, NULL);
        sceKernelDeleteThread(loading_thid);
        loading_thid = -1;
    }
}

int Loading_StartActivityThread()
{
    loading_thread_run = 0;
    Loading_WaitActivityThreadEnd();

    loading_thid = sceKernelCreateThread("loading_thread", loadingThreadFunc, 0x10000100, 0x10000, 0, 0, NULL);
    if (loading_thid >= 0)
    {
        loading_thread_run = 1;
        sceKernelStartThread(loading_thid, 0, NULL);
    }

    return loading_thid;
}

int Loading_ExitActivityThread()
{
    loading_thread_run = 0;
    Loading_WaitActivityThreadEnd();

    return 0;
}
