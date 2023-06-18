#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/system_param.h>
#include <psp2/shellutil.h>
#include <psp2/rtc.h>
#include <psp2/io/fcntl.h>

#include "emu/emu.h"
#include "utils.h"
#include "config.h"

extern char *_newlib_heap_base, *_newlib_heap_end, *_newlib_heap_cur;

static int home_locked = 0, usb_connection_locked = 0, quick_menu_locked = 0;
static int suspend_locked = 0, oled_off_locked = 0, oled_dimming_locked = 0;

static int psbutton_event_enabled = 1;

int AppLog(char *text, ...)
{
    if (!app_config.app_log)
        return 0;

    va_list list;
    char buf[512];

    va_start(list, text);
    vsprintf(buf, text, list);
    va_end(list);

#ifdef DEBUG
    printf(buf);
#endif

    SceUID fd = sceIoOpen((APP_LOG_PATH), SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd < 0)
        return fd;

    sceIoWrite(fd, buf, strlen(buf));
    sceIoClose(fd);

    return 0;
}

int HasEndSlash(const char *path)
{
    return path[strlen(path) - 1] == '/';
}

int RemoveEndSlash(char *path)
{
    int len = strlen(path);

    if (path[len - 1] == '/')
    {
        path[len - 1] = '\0';
        return 1;
    }

    return 0;
}

int AddEndSlash(char *path)
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

void ConvertUtcToLocalTime(SceDateTime *time_local, SceDateTime *time_utc)
{
    SceRtcTick tick;
    sceRtcGetTick(time_utc, &tick);
    sceRtcConvertUtcToLocalTime(&tick, &tick);
    sceRtcSetTick(time_local, &tick);
}

void ConvertLocalTimeToUtc(SceDateTime *time_utc, SceDateTime *time_local)
{
    SceRtcTick tick;
    sceRtcGetTick(time_local, &tick);
    sceRtcConvertLocalTimeToUtc(&tick, &tick);
    sceRtcSetTick(time_utc, &tick);
}

void GetSizeString(char string[16], uint64_t size)
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

void GetDateString(char string[24], int date_format, SceDateTime *time)
{
    SceDateTime time_local;
    ConvertUtcToLocalTime(&time_local, time);

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

void GetTimeString(char string[16], int time_format, SceDateTime *time)
{
    SceDateTime time_local;
    ConvertUtcToLocalTime(&time_local, time);

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

void GetDurationString(char string[16], uint64_t ms)
{
    static uint64_t ms_per_second = 1000;
    static uint64_t ms_per_minute = 60 * 1000;
    static uint64_t ms_per_hour = 60 * 60 * 1000;

    int hour = 0, minute = 0, second = 0;
    uint64_t remaining = ms;

    if (remaining > ms_per_hour)
    {
        hour = remaining / ms_per_hour;
        remaining -= hour * ms_per_hour;
    }
    if (remaining > ms_per_minute)
    {
        minute = remaining / ms_per_minute;
        remaining -= minute * ms_per_minute;
    }
    if (remaining > ms_per_second)
    {
        second = remaining / ms_per_second;
        // remaining -= second * ms_per_second;
    }

    if (hour > 0)
        snprintf(string, 16, "%02d:%02d:%02d", hour, minute, second);
    else
        snprintf(string, 16, "%02d:%02d", minute, second);
}

void RefreshListPos(int *top_pos, int *focus_pos, int length, int lines)
{
    int temp_top_pos = *top_pos;
    int temp_focus_pos = *focus_pos;
    int half_lines = lines / 2;

    if (temp_focus_pos > length - 1)
        temp_focus_pos = length - 1;
    if (temp_focus_pos < 0)
        temp_focus_pos = 0;

    temp_top_pos = temp_focus_pos - half_lines;

    if (temp_top_pos > length - lines)
        temp_top_pos = length - lines;
    if (temp_top_pos < 0)
        temp_top_pos = 0;

    *top_pos = temp_top_pos;
    *focus_pos = temp_focus_pos;
}

void RefreshListPosNoFocus(int *top_pos, int length, int lines)
{
    int temp_top_pos = *top_pos;

    if (temp_top_pos > length - lines)
        temp_top_pos = length - lines;
    if (temp_top_pos < 0)
        temp_top_pos = 0;

    *top_pos = temp_top_pos;
}

void MoveListPos(int type, int *top_pos, int *focus_pos, int length, int lines)
{
    int temp_top_pos = *top_pos;
    int temp_focus_pos = *focus_pos;
    if (type == TYPE_MOVE_UP)
        temp_focus_pos--;
    else if (type == TYPE_MOVE_DOWN)
        temp_focus_pos++;
    if (type == TYPE_MOVE_LEFT)
        temp_focus_pos -= lines;
    else if (type == TYPE_MOVE_RIGHT)
        temp_focus_pos += lines;

    RefreshListPos(&temp_top_pos, &temp_focus_pos, length, lines);
    *top_pos = temp_top_pos;
    *focus_pos = temp_focus_pos;
}

void MoveListPosNoFocus(int type, int *top_pos, int length, int lines)
{
    int temp_top_pos = *top_pos;
    if (type == TYPE_MOVE_UP)
        temp_top_pos--;
    else if (type == TYPE_MOVE_DOWN)
        temp_top_pos++;
    if (type == TYPE_MOVE_LEFT)
        temp_top_pos -= lines;
    else if (type == TYPE_MOVE_RIGHT)
        temp_top_pos += lines;

    RefreshListPosNoFocus(&temp_top_pos, length, lines);
    *top_pos = temp_top_pos;
}

void LockHomeKey()
{
    if (!home_locked)
    {
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN);
        home_locked = 1;
    }
}

void UnlockHomeKey()
{
    if (home_locked)
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_PS_BTN);
        home_locked = 0;
    }
}

void LockUsbConnection()
{
    if (!usb_connection_locked)
    {
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_USB_CONNECTION);
        usb_connection_locked = 1;
    }
}

void UnlockUsbConnection()
{
    if (usb_connection_locked)
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_USB_CONNECTION);
        usb_connection_locked = 0;
    }
}

void LockQuickMenu()
{
    if (!quick_menu_locked)
    {
        sceShellUtilLock(SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
        quick_menu_locked = 1;
    }
}

void UnlockQuickMenu()
{
    if (quick_menu_locked)
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
        quick_menu_locked = 0;
    }
}

void AutoUnlockQuickMenu()
{
    if (quick_menu_locked && !Emu_IsGameLoaded() && !Emu_IsGameLoading() && IsPSbuttonEventEnabled())
    {
        sceShellUtilUnlock(SCE_SHELL_UTIL_LOCK_TYPE_QUICK_MENU);
        quick_menu_locked = 0;
    }
}

void LockSuspend()
{
    if (suspend_locked < 0)
        suspend_locked = 1;
    else
        suspend_locked++;
}

void UnlockSuspend()
{
    if (suspend_locked > 0)
        suspend_locked--;
    else if (suspend_locked < 0)
        suspend_locked = 0;
}

void LockOledOff()
{
    if (oled_off_locked < 0)
        oled_off_locked = 1;
    else
        oled_off_locked++;
}

void UnlockOledOff()
{
    if (oled_off_locked > 0)
        oled_off_locked--;
    else if (oled_off_locked < 0)
        oled_off_locked = 0;
}

void LockOledDimming()
{
    if (oled_dimming_locked < 0)
        oled_dimming_locked = 1;
    else
        oled_dimming_locked++;
}

void UnlockOledDimming()
{
    if (oled_dimming_locked > 0)
        oled_dimming_locked--;
    else if (oled_dimming_locked < 0)
        oled_dimming_locked = 0;
}

void SetPSbuttonEventEnabled(int enable)
{
    psbutton_event_enabled = enable;
}

int IsPSbuttonEventEnabled()
{
    return psbutton_event_enabled;
}

static int powerTickThreadFunc(SceSize args, void *argp)
{
    while (1)
    {
        if (suspend_locked > 0 || oled_off_locked > 0 || oled_dimming_locked > 0)
        {
            int type = SCE_KERNEL_POWER_TICK_DEFAULT;
            if (suspend_locked > 0)
                type |= SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND;
            if (oled_off_locked > 0)
                type |= SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF;
            if (oled_dimming_locked > 0)
                type |= SCE_KERNEL_POWER_TICK_DISABLE_OLED_DIMMING;
            sceKernelPowerTick(type);
        }
        sceKernelDelayThread(10 * 1000 * 1000);
    }

    return 0;
}

void InitPowerTickThread()
{
    SceUID thid = sceKernelCreateThread("powerTickThreadFunc", powerTickThreadFunc, 0x10000100, 0x40000, 0, 0, NULL);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);
}

uint64_t GetTotalHeadMemSize()
{
    return _newlib_heap_end - _newlib_heap_base;
}

uint64_t GetFreeHeadMemSize()
{
    return _newlib_heap_end - _newlib_heap_cur;
}
