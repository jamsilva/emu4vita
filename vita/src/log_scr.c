#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/kernel/threadmgr.h>

#include "List/text_list.h"
#include "Gui/gui.h"
#include "log_scr.h"
#include "utils.h"

#define LISTVIEW_PADDING_L 10.0f
#define LISTVIEW_PADDING_T 10.0f

#define ITEMVIEW_PADDING_L 4.0f
#define ITEMVIEW_PADDING_T 4.0f

#define LISTVIEW_COLOR_BG COLOR_ALPHA(COLOR_BLACK, 0xCF)
#define TEXT_COLOR COLOR_WHITE

static SceUID log_thid = -1;
static int log_thread_stop = 0;
static SceKernelLwMutexWork log_mutex;

static TextList log_text_list = {0};
static int list_auto_scroll = 0;
static int list_max_draw_len = 0;
static int list_top_pos = 0;

static int listview_width, listview_height;
static int listview_sx, listview_sy, listview_dx, listview_dy;
static int itemview_width, itemview_height;

static void refreshLayout()
{
    listview_width = GUI_SCREEN_WIDTH;
    listview_height = GUI_SCREEN_HEIGHT;
    listview_sx = (GUI_SCREEN_WIDTH - listview_width) / 2;
    listview_sy = (GUI_SCREEN_HEIGHT - listview_height) / 2;
    listview_dx = listview_sx + listview_width;
    listview_dy = listview_sy + listview_height;

    itemview_width = listview_width - LISTVIEW_PADDING_L * 2;
    itemview_height = GUI_GetFontSize() + ITEMVIEW_PADDING_T * 2;

    list_max_draw_len = (listview_height - LISTVIEW_PADDING_T * 2) / itemview_height;
}

static void LogScrDisplay()
{
    sceKernelLockLwMutex(&log_mutex, 1, NULL);
    GUI_StartDrawing();

    GUI_DrawSplash();
    GUI_DrawFillRectangle(listview_sx, listview_sy, listview_width, listview_height, LISTVIEW_COLOR_BG);

    TextListEntry *entry = TextListGetEntryByNumber(&log_text_list, list_top_pos);

    if (entry)
    {
        int itemview_sx = listview_sx + LISTVIEW_PADDING_L;
        int itemview_sy = listview_sy + LISTVIEW_PADDING_T;
        int sx = itemview_sx + ITEMVIEW_PADDING_L;
        int sy;
        int drawn_len = 0;

        int i;
        for (i = list_top_pos; i < log_text_list.length; i++)
        {
            if (drawn_len >= list_max_draw_len)
                break;

            sy = itemview_sy + ITEMVIEW_PADDING_T;
            GUI_DrawText(sx, sy, TEXT_COLOR, entry->text);
            itemview_sy += itemview_height;
            drawn_len++;
            entry = entry->next;
        }

        GUI_DrawVerticalScrollbar(listview_dx - GUI_SCROLLBAR_SIZE, listview_sy, listview_height,
                                  log_text_list.length, list_max_draw_len, list_top_pos, 0);
    }

    GUI_EndDrawing();
    sceKernelUnlockLwMutex(&log_mutex, 1);
}

static void LogScrCtrl()
{
    sceKernelLockLwMutex(&log_mutex, 1, NULL);

    if (list_auto_scroll)
    {
        list_top_pos = log_text_list.length;
        RefreshListPosNoFocus(&list_top_pos, log_text_list.length, list_max_draw_len);
    }
    else
    {
        GUI_ReadPad();

        if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
            MoveListPosNoFocus(TYPE_MOVE_UP, &list_top_pos, log_text_list.length, list_max_draw_len);
        else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
            MoveListPosNoFocus(TYPE_MOVE_DOWN, &list_top_pos, log_text_list.length, list_max_draw_len);

        if (released_pad[PAD_CANCEL] || released_pad[PAD_PSBUTTON])
            log_thread_stop = 1;
    }

    sceKernelUnlockLwMutex(&log_mutex, 1);
}

static int LogPrintThreadFunc(SceSize args, void *argp)
{
    refreshLayout();
    list_top_pos = 0;

    while (!log_thread_stop)
    {
        LogScrDisplay();
        LogScrCtrl();
    }

    sceKernelDeleteLwMutex(&log_mutex);
    return 0;
}

int LogScrInit()
{
    if (log_thid >= 0)
        LogScrDeinit();

    log_thread_stop = 0;
    sceKernelCreateLwMutex(&log_mutex, "log_scr_mutex", 2, 0, NULL);
    log_thid = sceKernelCreateThread("log_scr_thread", LogPrintThreadFunc, 0x10000100, 0x10000, 0, 0, NULL);
    if (log_thid >= 0)
        sceKernelStartThread(log_thid, 0, NULL);
    else
        sceKernelDeleteLwMutex(&log_mutex);

    return log_thid;
}

int LogScrWaitExit()
{
    if (log_thid >= 0)
    {
        sceKernelWaitThreadEnd(log_thid, NULL, NULL);
        sceKernelDeleteThread(log_thid);
        sceKernelDeleteLwMutex(&log_mutex);
        TextListEmpty(&log_text_list);
        log_thid = -1;
    }

    return 0;
}

int LogScrDeinit()
{
    log_thread_stop = 1;
    if (log_thid >= 0)
        LogScrWaitExit();

    return 0;
}

void LogScrPrintText(const char *text)
{
    sceKernelLockLwMutex(&log_mutex, 1, NULL);
    TextListAddEntryEx(&log_text_list, text);
    sceKernelUnlockLwMutex(&log_mutex, 1);
}

void LogScrPrintfText(const char *text, ...)
{
    char buf[1024];
    va_list argptr;
    va_start(argptr, text);
    vsnprintf(buf, sizeof(buf), text, argptr);
    va_end(argptr);

    LogScrPrintText(buf);
}

void LogScrSetAutoScroll(int enable)
{
    list_auto_scroll = enable;
}
