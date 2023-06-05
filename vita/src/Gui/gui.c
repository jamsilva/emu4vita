#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/system_param.h>
#include <psp2/power.h>
#include <psp2/ctrl.h>

#include "Emu/emu.h"
#include "gui.h"
#include "utils.h"
#include "lang.h"
#include "init.h"

extern GUI_Activity browser_activity;
extern GUI_Dialog setting_dialog;

static GUI_Activity *main_activity = &browser_activity;
static GUI_Activity *current_activity = NULL;
static GUI_Dialog *current_dialog = NULL;

// Images
static GUI_Texture *wallpaper_texture = NULL;
static float wallpaper_x_scale, wallpaper_y_scale;
static GUI_Texture *splash_texture = NULL;
static float splash_x_scale, splash_y_scale;

// Status bar
#define STATUS_BAR_PADDING_T 10.0f
#define STATUS_BAR_PADDING_L 10.0f
static int statusbar_width, statusbar_height;

// Free draw
#define ACTIVITY_FREE_DRAW_PADDING_L 10.0f
#define ACTIVITY_FREE_DRAW_PADDING_T 10.0f
float ACTIVITY_FREE_DRAW_WIDTH, ACTIVITY_FREE_DRAW_HEIGHT;
float ACTIVITY_FREE_DRAW_SX, ACTIVITY_FREE_DRAW_SY, ACTIVITY_FREE_DRAW_DX, ACTIVITY_FREE_DRAW_DY;

// Button
char STR_BUTTON_ENTER[4], STR_BUTTON_CANCEL[4];

void GUI_RefreshLayout()
{
    statusbar_width = GUI_SCREEN_WIDTH;
    statusbar_height = STATUS_BAR_PADDING_T * 2 + GUI_GetFontSize();

    ACTIVITY_FREE_DRAW_WIDTH = GUI_SCREEN_WIDTH - ACTIVITY_FREE_DRAW_PADDING_L * 2;
    ACTIVITY_FREE_DRAW_HEIGHT = GUI_SCREEN_HEIGHT - statusbar_height * 2 - ACTIVITY_FREE_DRAW_PADDING_T * 2;
    ACTIVITY_FREE_DRAW_SX = ACTIVITY_FREE_DRAW_PADDING_L;
    ACTIVITY_FREE_DRAW_SY = statusbar_height + ACTIVITY_FREE_DRAW_PADDING_T;
    ACTIVITY_FREE_DRAW_DX = ACTIVITY_FREE_DRAW_SX + ACTIVITY_FREE_DRAW_WIDTH;
    ACTIVITY_FREE_DRAW_DY = ACTIVITY_FREE_DRAW_SY + ACTIVITY_FREE_DRAW_HEIGHT;
}

void GUI_GetEnterButton()
{
    if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE)
    {
        strcpy(STR_BUTTON_ENTER, STR_BUTTON_CIRCLE);
        strcpy(STR_BUTTON_CANCEL, STR_BUTTON_CROSS);
    }
    else
    {
        strcpy(STR_BUTTON_ENTER, STR_BUTTON_CROSS);
        strcpy(STR_BUTTON_CANCEL, STR_BUTTON_CIRCLE);
    }
}

int GUI_SetMainActivity(GUI_Activity *activity)
{
    if (!activity)
        return -1;

    main_activity = activity;
    main_activity->parent = NULL;

    return 0;
}

int GUI_BacktoMainActivity()
{
    if (!main_activity)
        return -1;

    main_activity->parent = NULL;
    current_activity = main_activity;

    return 0;
}

int GUI_EnterActivity(GUI_Activity *activity)
{
    if (!activity || current_activity == activity)
        return -1;

    activity->parent = current_activity;
    current_activity = activity;
    if (current_activity->init)
        current_activity->init();

    return 0;
}

int GUI_ExitActivity(GUI_Activity *activity)
{
    if (!activity || current_activity != activity)
        return -1;

    if (activity->parent)
    {
        current_activity = activity->parent;
        activity->parent = NULL;
    }

    return 0;
}

int GUI_OpenDialog(GUI_Dialog *dialog)
{
    if (!dialog || current_dialog == dialog)
        return -1;

    dialog->parent = current_dialog;
    current_dialog = dialog;
    if (current_dialog && current_dialog->open)
        current_dialog->open();

    return 0;
}

int GUI_CloseDialog(GUI_Dialog *dialog)
{
    if (!dialog || current_dialog != dialog)
        return -1;

    if (current_dialog && current_dialog->close)
        current_dialog->close();
    current_dialog = dialog->parent;
    dialog->parent = NULL;

    return 0;
}

void GUI_DrawTopStatusBar(char *title)
{
    float view_sx = 0;
    float view_dx = GUI_SCREEN_WIDTH;
    float view_sy = 0;

    GUI_DrawFillRectangle(view_sx, view_sy, statusbar_width, statusbar_height, GUI_DEFALUT_BG_COLOR);

    int sx = view_sx + STATUS_BAR_PADDING_L;
    int sy = view_sy + STATUS_BAR_PADDING_T;
    GUI_DrawText(sx, sy, COLOR_WHITE, title);

    sx = view_dx - STATUS_BAR_PADDING_L;
    if (!is_vitatv_model)
    {
        uint32_t color;
        if (scePowerIsBatteryCharging())
            color = COLOR_YELLOW;
        else if (scePowerIsLowBattery())
            color = COLOR_RED;
        else
            color = COLOR_GREEN;

        int percent = scePowerGetBatteryLifePercent();
        char battery_string[24];
        snprintf(battery_string, sizeof(battery_string), "%s: %d%%", STR_BATTERY, percent);
        float battery_x = sx - GUI_GetTextWidth(battery_string);
        GUI_DrawText(battery_x, sy, color, battery_string);
        sx = battery_x - STATUS_BAR_PADDING_L;
    }

    // Date & time
    SceDateTime time;
    sceRtcGetCurrentClock(&time, 0);

    char date_string[24];
    GetDateString(date_string, date_format, &time);

    char time_string[16];
    GetTimeString(time_string, time_format, &time);

    char string[64];
    snprintf(string, sizeof(string), "%s  %s", date_string, time_string);
    float date_time_x = sx - GUI_GetTextWidth(string);
    GUI_DrawText(date_time_x, sy, GUI_DEFALUT_TEXT_COLOR, string);
}

void GUI_DrawBottomStatusBar(GUI_ButtonInstruction *instructions)
{
    float view_sx = 0;
    float view_sy = GUI_SCREEN_HEIGHT - statusbar_height;

    GUI_DrawFillRectangle(view_sx, view_sy, statusbar_width, statusbar_height, GUI_DEFALUT_BG_COLOR);

    float sx = view_sx + STATUS_BAR_PADDING_L;
    float sy = view_sy + STATUS_BAR_PADDING_T;
    int i;
    for (i = 0; instructions[i].button; i++)
    {
        sx += GUI_DrawTextf(sx, sy, COLOR_AZURE, "%s:", instructions[i].button);
        sx += GUI_DrawText(sx, sy, COLOR_WHITE, instructions[i].instruction);
        sx += STATUS_BAR_PADDING_L;
    }
}

void GUI_DrawVerticalScrollbar(int track_sx, int track_sy, int track_height, int list_len, int max_draw_len, int top_pos, int draw_track)
{
    if (list_len > max_draw_len)
    {
        // Draw scroll track
        if (draw_track)
            GUI_DrawFillRectangle(track_sx, track_sy, GUI_SCROLLBAR_SIZE, track_height, GUI_SCROLLBAR_TRACK_COLOR);

        // Draw scroll thumb
        float size_per_item = (float)track_height / (float)list_len;
        int thumb_height = (float)max_draw_len * size_per_item;
        thumb_height = MAX(thumb_height, 1); // Fix
        int thumb_sy;
        if (top_pos >= list_len - max_draw_len)
        {
            thumb_sy = track_sy + track_height - thumb_height;
        }
        else
        {
            thumb_sy = track_sy + (float)top_pos * size_per_item;
            thumb_sy = MIN(thumb_sy, (track_sy + track_height - thumb_height)); // Fix
        }
        GUI_DrawFillRectangle(track_sx, thumb_sy, GUI_SCROLLBAR_SIZE, thumb_height, GUI_SCROLLBAR_THUMB_COLOR);
    }
}

void GUI_SetWallpaperTexture(GUI_Texture *texture)
{
    GUI_WaitRenderDone();
    if (wallpaper_texture)
        vita2d_free_texture(wallpaper_texture);
    wallpaper_texture = NULL;

    if (texture)
    {
        wallpaper_x_scale = GUI_SCREEN_WIDTH / (float)GUI_GetTextureWidth(texture);
        wallpaper_y_scale = GUI_SCREEN_HEIGHT / (float)GUI_GetTextureHeight(texture);
        wallpaper_texture = texture;
    }
}

void GUI_SetSplashTexture(GUI_Texture *texture)
{
    GUI_WaitRenderDone();
    if (splash_texture)
        vita2d_free_texture(splash_texture);
    splash_texture = NULL;

    if (texture)
    {
        splash_x_scale = GUI_SCREEN_WIDTH / (float)GUI_GetTextureWidth(texture);
        splash_y_scale = GUI_SCREEN_HEIGHT / (float)GUI_GetTextureHeight(texture);
        splash_texture = texture;
    }
}

void GUI_DisplaySafeMode()
{
    while (1)
    {
        int x = 30, y = 30;
        GUI_StartDrawing();
        GUI_DrawText(x, y, COLOR_WHITE, STR_SAFE_MODE_PRINT_0);
        y += GUI_GetFontSize();
        GUI_DrawText(x, y, COLOR_WHITE, STR_SAFE_MODE_PRINT_1);
        y += GUI_GetFontSize() * 2;
        GUI_DrawText(x, y, COLOR_WHITE, STR_SAFE_MODE_PRINT_2);
        GUI_EndDrawing();

        SceCtrlData pad;
        memset(&pad, 0, sizeof(SceCtrlData));
        sceCtrlPeekBufferPositiveExt2(0, &pad, 1);

        if (pad.buttons & ~SCE_CTRL_INTERCEPTED)
            break;
    }
}

void GUI_DrawWallpaper()
{
    if (wallpaper_texture)
        GUI_DrawTextureScale(wallpaper_texture, 0.0f, 0.0f, wallpaper_x_scale, wallpaper_y_scale);
}

void GUI_DrawSplash()
{
    if (splash_texture)
        GUI_DrawTextureScale(splash_texture, 0.0f, 0.0f, splash_x_scale, splash_y_scale);
}

void GUI_DisplaySplash()
{
    GUI_StartDrawing();
    GUI_DrawSplash();
    GUI_EndDrawing();
}

static void drawActivity()
{
    if (!current_activity->nowallpaper)
        GUI_DrawWallpaper();

    if (current_activity->draw)
        current_activity->draw();

    if (!current_activity->nostatusbar)
    {
        GUI_DrawTopStatusBar(current_activity->title);
        GUI_DrawBottomStatusBar(current_activity->button_instructions);
    }
}

void GUI_DrawMain()
{
    if (Emu_IsGameLoaded())
        Emu_DrawVideo();
    else
        drawActivity();

    if (current_dialog && current_dialog->draw)
        current_dialog->draw();
}

static void ctrlCommon()
{
    if (released_pad[PAD_PSBUTTON])
    {
        if (IsPSbuttonEventEnabled())
        {
            if (current_activity == main_activity)
            {
                if (current_dialog != &setting_dialog)
                {
                    GUI_OpenDialog(&setting_dialog);
                    SetPSbuttonEventEnabled(0);
                }
            }
            else
            {
                GUI_BacktoMainActivity();
            }
        }
    }
}

void GUI_CtrlMain()
{
    ctrlCommon();

    if (current_dialog && current_dialog->ctrl)
        current_dialog->ctrl();
    else if (current_activity->ctrl)
        current_activity->ctrl();
}

void GUI_RunMain()
{
    GUI_StartDrawing();
    GUI_DrawMain();
    GUI_EndDrawing();
    GUI_ReadPad();
    GUI_CtrlMain();
    UnlockQuickMenu();
}
