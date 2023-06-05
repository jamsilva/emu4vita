#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/rtc.h>
#include <psp2/shellutil.h>
#include <psp2/system_param.h>
#include <psp2/io/fcntl.h>
#include <psp2/gxm.h>
#include <psp2/touch.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>

#include "utils.h"
#include "ui.h"
#include "init.h"
#include "browser.h"
#include "ui.h"
#include "config.h"
#include "file.h"

PadList pad_list[4];
Pad old_pads[4], current_pads[4];
Pad hold_counts[4];
Pad old_pad, current_pad, pressed_pad, released_pad, hold_pad, hold2_pad;
Pad hold_count, hold2_count;

static int home_locked = 0;
static int usb_connection_locked = 0;
static int quick_menu_locked = 0;
static int skip_pressed_psbutton = 0;

static int app_log_inited = 0;
static SceKernelLwMutexWork app_log_mutex;

static void initAppLog()
{
    if (!app_log_inited)
    {
        sceKernelCreateLwMutex(&app_log_mutex, "app_log_mutex", 2, 0, NULL);
        app_log_inited = 1;
    }
}

int APP_LOG(char *text, ...)
{
    if (!app_log_inited)
        initAppLog();

    sceKernelLockLwMutex(&app_log_mutex, 1, NULL);

    va_list list;
    char string[512];

    va_start(list, text);

    vsprintf(string, text, list);
    va_end(list);

    SceUID fd = sceIoOpen((APP_LOG_PATH), SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd < 0)
        return fd;

    sceIoWrite(fd, string, strlen(string));
    sceIoClose(fd);

    sceKernelUnlockLwMutex(&app_log_mutex, 1);

    return 0;
}

void readPad()
{
    int port, i;

    memcpy(&old_pad, current_pad, sizeof(Pad));
    memset(&current_pad, 0, sizeof(Pad));

    for (i = 0; i < 3; i++)
    {
        if (i > 0)
            port = i + 1;
        else
            port = i;

        memset(&current_pads[i], 0, sizeof(Pad));
        memset(&pad_list[i].pad, 0, sizeof(SceCtrlData));
        int ret = sceCtrlPeekBufferPositiveExt2(port, &pad_list[i].pad, 1);
        if (ret < 0)
        {
            pad_list[i].error = 1;
            continue;
        }

        // if (CHECK_EXIT_APP(pad_list[i].pad.buttons))
        // exitApp();

        if (pad_list[i].pad.buttons & SCE_CTRL_UP)
            current_pad[PAD_UP] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_DOWN)
            current_pad[PAD_DOWN] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_LEFT)
            current_pad[PAD_LEFT] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_RIGHT)
            current_pad[PAD_RIGHT] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_TRIANGLE)
            current_pad[PAD_TRIANGLE] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_CIRCLE)
            current_pad[PAD_CIRCLE] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_CROSS)
            current_pad[PAD_CROSS] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_SQUARE)
            current_pad[PAD_SQUARE] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_L1)
            current_pad[PAD_L1] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_R1)
            current_pad[PAD_R1] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_L2)
            current_pad[PAD_L2] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_R2)
            current_pad[PAD_R2] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_L3)
            current_pad[PAD_L3] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_R3)
            current_pad[PAD_R3] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_START)
            current_pad[PAD_START] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_SELECT)
            current_pad[PAD_SELECT] = 1;
        if (pad_list[i].pad.buttons & SCE_CTRL_PSBUTTON)
            current_pad[PAD_PSBUTTON] = 1;

        if (pad_list[i].pad.lx < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_LEFT] = 1;
        else if (pad_list[i].pad.lx > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_RIGHT] = 1;

        if (pad_list[i].pad.ly < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_UP] = 1;
        else if (pad_list[i].pad.ly > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_DOWN] = 1;

        if (pad_list[i].pad.rx < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_LEFT] = 1;
        else if (pad_list[i].pad.rx > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_RIGHT] = 1;

        if (pad_list[i].pad.ry < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_UP] = 1;
        else if (pad_list[i].pad.ry > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_DOWN] = 1;

        if (current_pad[PAD_PSBUTTON])
        {
            hold_counts[i][PAD_PSBUTTON]++;
            if (hold_counts[i][PAD_PSBUTTON] >= SKIP_PRESSED_PSBUTTON_COUNT)
                skipPressedPsbutton();
        }
        else
        {
            hold_counts[i][PAD_PSBUTTON] = 0;
            if (!old_pad[PAD_PSBUTTON])
                unskipPressedPsbutton();
        }
    }

    for (i = 0; i < PAD_N_BUTTONS; i++)
    {
        pressed_pad[i] = current_pad[i] & ~old_pad[i];
        released_pad[i] = ~current_pad[i] & old_pad[i];

        hold_pad[i] = pressed_pad[i];
        hold2_pad[i] = pressed_pad[i];

        if (current_pad[i])
        {
            if (hold_count[i] >= 10)
            {
                hold_pad[i] = 1;
                hold_count[i] = 6;
            }

            if (hold2_count[i] >= 10)
            {
                hold2_pad[i] = 1;
                hold2_count[i] = 10;
            }

            hold_count[i]++;
            hold2_count[i]++;
        }
        else
        {
            hold_count[i] = 0;
            hold2_count[i] = 0;
        }
    }

    if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE)
    {
        old_pad[PAD_ENTER] = old_pad[PAD_CIRCLE];
        current_pad[PAD_ENTER] = current_pad[PAD_CIRCLE];
        pressed_pad[PAD_ENTER] = pressed_pad[PAD_CIRCLE];
        released_pad[PAD_ENTER] = released_pad[PAD_CIRCLE];
        hold_pad[PAD_ENTER] = hold_pad[PAD_CIRCLE];
        hold2_pad[PAD_ENTER] = hold2_pad[PAD_CIRCLE];

        old_pad[PAD_CANCEL] = old_pad[PAD_CROSS];
        current_pad[PAD_CANCEL] = current_pad[PAD_CROSS];
        pressed_pad[PAD_CANCEL] = pressed_pad[PAD_CROSS];
        released_pad[PAD_CANCEL] = released_pad[PAD_CROSS];
        hold_pad[PAD_CANCEL] = hold_pad[PAD_CROSS];
        hold2_pad[PAD_CANCEL] = hold2_pad[PAD_CROSS];
    }
    else
    {
        old_pad[PAD_ENTER] = old_pad[PAD_CROSS];
        current_pad[PAD_ENTER] = current_pad[PAD_CROSS];
        pressed_pad[PAD_ENTER] = pressed_pad[PAD_CROSS];
        released_pad[PAD_ENTER] = released_pad[PAD_CROSS];
        hold_pad[PAD_ENTER] = hold_pad[PAD_CROSS];
        hold2_pad[PAD_ENTER] = hold2_pad[PAD_CROSS];

        old_pad[PAD_CANCEL] = old_pad[PAD_CIRCLE];
        current_pad[PAD_CANCEL] = current_pad[PAD_CIRCLE];
        pressed_pad[PAD_CANCEL] = pressed_pad[PAD_CIRCLE];
        released_pad[PAD_CANCEL] = released_pad[PAD_CIRCLE];
        hold_pad[PAD_CANCEL] = hold_pad[PAD_CIRCLE];
        hold2_pad[PAD_CANCEL] = hold2_pad[PAD_CIRCLE];
    }
}

void resetPad()
{
    memset(&old_pad, 0, sizeof(Pad));
    memset(&current_pad, 0, sizeof(Pad));
    memset(&pressed_pad, 0, sizeof(Pad));
    memset(&released_pad, 0, sizeof(Pad));
    memset(&hold_pad, 0, sizeof(Pad));
    memset(&hold2_pad, 0, sizeof(Pad));
}

int hasEndSlash(const char *path)
{
    return path[strlen(path) - 1] == '/';
}

int removeEndSlash(char *path)
{
    int len = strlen(path);

    if (path[len - 1] == '/')
    {
        path[len - 1] = '\0';
        return 1;
    }

    return 0;
}

int addEndSlash(char *path)
{
    int len = strlen(path);
    if (len < MAX_PATH_LENGTH - 2)
    {
        if (path[len - 1] != '/')
        {
            path[len] = '/';
            path[len + 1] = '\0';
            return 1;
        }
    }

    return 0;
}

void convertUtcToLocalTime(SceDateTime *time_local, SceDateTime *time_utc)
{
    SceRtcTick tick;
    sceRtcGetTick(time_utc, &tick);
    sceRtcConvertUtcToLocalTime(&tick, &tick);
    sceRtcSetTick(time_local, &tick);
}

void convertLocalTimeToUtc(SceDateTime *time_utc, SceDateTime *time_local)
{
    SceRtcTick tick;
    sceRtcGetTick(time_local, &tick);
    sceRtcConvertLocalTimeToUtc(&tick, &tick);
    sceRtcSetTick(time_utc, &tick);
}

void getSizeString(char string[16], uint64_t size)
{
    double double_size = (double)size;

    int i = 0;
    static char *units[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (double_size >= 1024.0)
    {
        double_size /= 1024.0;
        i++;
    }

    snprintf(string, 16, "%.*f %s", (i == 0) ? 0 : 2, double_size, units[i]);
}

void getDateString(char string[24], int date_format, SceDateTime *time)
{
    SceDateTime time_local;
    convertUtcToLocalTime(&time_local, time);

    switch (date_format)
    {
    case SCE_SYSTEM_PARAM_DATE_FORMAT_YYYYMMDD:
        snprintf(string, 24, "%04d/%02d/%02d", time_local.year, time_local.month, time_local.day);
        break;

    case SCE_SYSTEM_PARAM_DATE_FORMAT_DDMMYYYY:
        snprintf(string, 24, "%02d/%02d/%04d", time_local.day, time_local.month, time_local.year);
        break;

    case SCE_SYSTEM_PARAM_DATE_FORMAT_MMDDYYYY:
        snprintf(string, 24, "%02d/%02d/%04d", time_local.month, time_local.day, time_local.year);
        break;
    }
}

void getTimeString(char string[16], int time_format, SceDateTime *time)
{
    SceDateTime time_local;
    convertUtcToLocalTime(&time_local, time);

    switch (time_format)
    {
    case SCE_SYSTEM_PARAM_TIME_FORMAT_12HR:
        snprintf(string, 16, "%02d:%02d %s", (time_local.hour > 12) ? (time_local.hour - 12) : ((time_local.hour == 0) ? 12 : time_local.hour),
                 time_local.minute, time_local.hour >= 12 ? "PM" : "AM");
        break;

    case SCE_SYSTEM_PARAM_TIME_FORMAT_24HR:
        snprintf(string, 16, "%02d:%02d", time_local.hour, time_local.minute);
        break;
    }
}

void refreshListPos(int *top_pos, int *focus_pos, int length, int lines)
{
    int temp_top_pos = *top_pos;
    int temp_focus_pos = *focus_pos;

    if (temp_focus_pos > length - 1)
    {
        temp_focus_pos = length - 1;
    }
    if (temp_focus_pos < 0)
    {
        temp_focus_pos = 0;
    }

    int lines_center = (int)((float)lines / 2 + 0.5f);
    temp_top_pos = temp_focus_pos - lines_center + 1;
    if (temp_top_pos > length - lines)
    {
        temp_top_pos = length - lines;
    }
    if (temp_top_pos < 0)
    {
        temp_top_pos = 0;
    }

    *top_pos = temp_top_pos;
    *focus_pos = temp_focus_pos;
}

void controlRefreshListPos(int type, int *top_pos, int *focus_pos, int length, int lines)
{
    int temp_focus_pos = *focus_pos;
    if (type == LIST_CONTROL_UP)
        temp_focus_pos--;
    else if (type == LIST_CONTROL_DOWN)
        temp_focus_pos++;
    if (type == LIST_CONTROL_SKIP_UP)
        temp_focus_pos -= lines;
    else if (type == LIST_CONTROL_SKIP_DOWN)
        temp_focus_pos += lines;

    refreshListPos(top_pos, &temp_focus_pos, length, lines);
    *focus_pos = temp_focus_pos;
}

static int power_tick_thread(SceSize args, void *argp)
{
    while (1)
    {
        if (home_locked > 0)
            sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
        sceKernelDelayThread(10 * 1000 * 1000);
    }

    return 0;
}

void initPowerTickThread()
{
    SceUID thid = sceKernelCreateThread("power_tick_thread", power_tick_thread, 0x10000100, 0x40000, 0, 0, NULL);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);
}

void lockHome()
{
    if (!home_locked)
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN);
    home_locked++;
}

void unlockHome()
{
    if (home_locked == 1)
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN);
    home_locked--;
    if (home_locked < 0)
        home_locked = 0;
}

void unlockHomeEx()
{
    sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN);
    home_locked = 0;
}

void lockUsbConnection()
{
    if (!usb_connection_locked)
    {
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_USB_CONNECTION);
        usb_connection_locked = 1;
    }
}

void unlockUsbConnection()
{
    if (usb_connection_locked)
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_USB_CONNECTION);
        usb_connection_locked = 0;
    }
}

void lockQuickMenu()
{
    if (!quick_menu_locked)
    {
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
        quick_menu_locked = 1;
    }
}

void unlockQuickMenu()
{
    if (quick_menu_locked && !current_pad[PAD_PSBUTTON])
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
        quick_menu_locked = 0;
    }
}

void skipPressedPsbutton()
{
    skip_pressed_psbutton = 1;
}

void unskipPressedPsbutton()
{
    skip_pressed_psbutton = 0;
}

int isSkipPressedPsbutton()
{
    return skip_pressed_psbutton;
}
