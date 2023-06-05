#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/system_param.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "ui.h"
#include "utils.h"
#include "browser.h"
#include "init.h"
#include "lang.h"
#include "config.h"
#include "file.h"

#define MAIN_PADDING 0.0f

#define FREE_DRAW_PADDING_T 0.0f;
#define FREE_DRAW_PADDING_L 0.0f;

#define STATUS_BAR_PADDING_T 10.0f
#define STATUS_BAR_PADDING_L 10.0f
#define STATUS_BAR_BG_COLOR COLOR_ALPHA(0x1F1F1F, 0x4F)

#define SCROLL_BAR_MIN_HEIGHT 4.0f
#define SCROLL_BAR_COLOR COLOR_ALPHA(LITEGRAY, 0x8F)
#define SCROLL_BAR_BG_COLOR COLOR_ALPHA(DARKGRAY, 0x8F)

#define MAIN_BG_COLOR COLOR_ALPHA(0x1F1F1F, 0x4F)

#define MAIN_TITLE APP_NAME_STR " " APP_VER_STR

char STR_BUTTON_ENTER[4], STR_BUTTON_CANCEL[4];

static vita2d_texture *wallpaper_tex = NULL;
static float wallpaper_x_scale, wallpaper_y_scale;

float MAIN_FREE_DRAW_PADDING_T, MAIN_FREE_DRAW_PADDING_L;
float MAIN_FREE_DRAW_WIDTH, MAIN_FREE_DRAW_HEIGHT;
float MAIN_FREE_DRAW_SX, MAIN_FREE_DRAW_DX, MAIN_FREE_DRAW_SY, MAIN_FREE_DRAW_DY;

static float status_bar_width, status_bar_height;

static void initUiDrawInfo()
{
    status_bar_width = SCREEN_WIDTH;
    status_bar_height = STATUS_BAR_PADDING_T * 2 + UiGetLineHeight();

    MAIN_FREE_DRAW_PADDING_T = FREE_DRAW_PADDING_T;
    MAIN_FREE_DRAW_PADDING_L = FREE_DRAW_PADDING_L;
    MAIN_FREE_DRAW_WIDTH = SCREEN_WIDTH - MAIN_PADDING * 2 - MAIN_FREE_DRAW_PADDING_T * 2;
    MAIN_FREE_DRAW_HEIGHT = SCREEN_HEIGHT - MAIN_PADDING * 2 - status_bar_height * 2 - MAIN_FREE_DRAW_PADDING_T * 2;
    MAIN_FREE_DRAW_SX = MAIN_PADDING + MAIN_FREE_DRAW_PADDING_L;
    MAIN_FREE_DRAW_DX = MAIN_FREE_DRAW_SX + MAIN_FREE_DRAW_WIDTH;
    MAIN_FREE_DRAW_SY = MAIN_PADDING + status_bar_height + MAIN_FREE_DRAW_PADDING_T;
    MAIN_FREE_DRAW_DY = SCREEN_HEIGHT - MAIN_PADDING - status_bar_height - MAIN_FREE_DRAW_PADDING_T;
}

static int InitImagesThreadCallback(SceSize args, void *argp)
{
    vita2d_texture *texture = vita2d_load_PNG_file(WALLPAPER_PNG_PATH);
    if (texture)
    {
        wallpaper_x_scale = SCREEN_WIDTH / (float)vita2d_texture_get_width(texture);
        wallpaper_y_scale = SCREEN_HEIGHT / (float)vita2d_texture_get_height(texture);
        wallpaper_tex = texture;
    }

    sceKernelExitDeleteThread(0);
    return 0;
}

void initImagesThread()
{
    SceUID thid = sceKernelCreateThread("init_images_thread", InitImagesThreadCallback, 0x10000100, 0x10000, 0, 0, NULL);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);
}

static void finishImages()
{
    if (wallpaper_tex)
    {
        vita2d_free_texture(wallpaper_tex);
        wallpaper_tex = NULL;
    }
}

void initDrawInfo()
{
    initUiDrawInfo();
    initBrowserDrawInfo();
}

void initUi()
{
    initImagesThread();
    initIconsThread();
    initUiFonts();
    initDrawInfo();
    initBrowser();
}

void finishUi()
{
    finishUiFonts();
    finishImages();
}

static void drawTopStatusBar(char *title)
{
    float view_sx = 0;
    float view_dx = SCREEN_WIDTH;
    float view_sy = 0;

    vita2d_draw_rectangle(view_sx, view_sy, status_bar_width, status_bar_height, STATUS_BAR_BG_COLOR);

    float sx = view_sx + STATUS_BAR_PADDING_L;
    float sy = view_sy + STATUS_BAR_PADDING_T;
    UiDrawText(sx, sy, WHITE, title);

    sx = view_dx - STATUS_BAR_PADDING_L;
    if (!is_vitatv_model)
    {
        uint32_t color;
        if (scePowerIsBatteryCharging())
            color = YELLOW;
        else if (scePowerIsLowBattery())
            color = RED;
        else
            color = GREEN;

        int percent = scePowerGetBatteryLifePercent();
        char battery_string[24];
        snprintf(battery_string, sizeof(battery_string), "%d%%", percent);
        float battery_x = sx - UiGetTextWidth(battery_string);
        UiDrawText(battery_x, sy, color, battery_string);
        sx = battery_x - STATUS_BAR_PADDING_L;
    }

    // Date & time
    SceDateTime time;
    sceRtcGetCurrentClock(&time, 0);

    char date_string[24];
    getDateString(date_string, date_format, &time);

    char time_string[16];
    getTimeString(time_string, time_format, &time);

    char string[64];
    snprintf(string, sizeof(string), "%s  %s", date_string, time_string);
    float date_time_x = sx - UiGetTextWidth(string);
    UiDrawText(date_time_x, sy, DEFALUT_FONT_COLOR, string);
}

static void drawBottomStatusBar()
{
    float view_sx = 0;
    float view_sy = SCREEN_HEIGHT - status_bar_height;

    vita2d_draw_rectangle(view_sx, view_sy, status_bar_width, status_bar_height, STATUS_BAR_BG_COLOR);

    float sx = view_sx + STATUS_BAR_PADDING_L;
    float sy = view_sy + STATUS_BAR_PADDING_T;
    UiDrawTextf(sx, sy, WHITE, "Built on  %s by Yizhigai", BUILD_DATE);
}

void drawScrollBar(float sx, float sy, float full_height, int max_lines, int list_len, int top_pos)
{
    if (list_len > max_lines)
    {
        // vita2d_draw_rectangle(sx, sy, SCROLL_BAR_WIDTH, full_height, SCROLL_BAR_BG_COLOR);

        float pcs_height = full_height / list_len;
        float scroll_bar_sy = sy + top_pos * pcs_height;
        float scroll_bar_height = max_lines * pcs_height;
        vita2d_draw_rectangle(sx, MIN(scroll_bar_sy, (sy + full_height - scroll_bar_height)), SCROLL_BAR_WIDTH,
                              MAX(scroll_bar_height, SCROLL_BAR_MIN_HEIGHT), SCROLL_BAR_COLOR);
    }
}

void printSafeMode()
{
    while (1)
    {
        int x = 30, y = 30;
        float line_height = UiGetLineHeight();
        UiStartDrawing();
        UiDrawText(x, y, WHITE, STR_SAFE_MODE_PRINT_0);
        y += line_height;
        UiDrawText(x, y, WHITE, STR_SAFE_MODE_PRINT_1);
        y += line_height * 2;
        UiDrawText(x, y, WHITE, STR_SAFE_MODE_PRINT_2);
        UiEndDrawing();

        SceCtrlData pad;
        memset(&pad, 0, sizeof(SceCtrlData));
        sceCtrlPeekBufferPositiveExt2(0, &pad, 1);

        if (pad.buttons & ~SCE_CTRL_INTERCEPTED)
            break;
    }
    finishMain();
    sceKernelExitProcess(0);
}

static void drawMain()
{
    if (wallpaper_tex)
        vita2d_draw_texture_scale(wallpaper_tex, 0.0f, 0.0f, wallpaper_x_scale, wallpaper_y_scale);

    vita2d_draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MAIN_BG_COLOR);

    drawBrowser();
    drawTopStatusBar(MAIN_TITLE);
    drawBottomStatusBar();
}

static void controlMain()
{
    ctrlBrowser();
}

void UiMain()
{
    readPad();
    UiStartDrawing();
    drawMain();
    UiEndDrawing();
    controlMain();
}
