#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/system_param.h>
#include <psp2/power.h>
#include <psp2/ctrl.h>

#include "emu/emu.h"
#include "gui.h"
#include "utils.h"
#include "lang.h"
#include "init.h"

// Status bar
#define STATUS_BAR_PADDING_T 10
#define STATUS_BAR_PADDING_L 10
#define STATUS_BAR_WIDTH GUI_SCREEN_WIDTH
#define STATUS_BAR_HEIGHT (GUI_GetFontSize() + STATUS_BAR_PADDING_T * 2)

extern GUI_Activity browser_activity;
extern GUI_Dialog setting_dialog;

static GUI_Activity *current_activity = NULL;
static GUI_Dialog *main_dialog = NULL, *current_dialog = NULL;

static GUI_Texture *wallpaper_texture = NULL, *splash_texture = NULL;

int GUI_StartActivity(GUI_Activity *activity)
{
    if (!activity || current_activity == activity)
        return -1;

    activity->parent = current_activity;
    if (activity->startCallback)
        activity->startCallback(activity);

    current_activity = activity;

    return 0;
}

int GUI_ExitActivity(GUI_Activity *activity)
{
    // Can not exit other activity
    if (!activity || current_activity != activity)
        return -1;

    // Can not exit main activity
    if (!activity->parent)
        return -1;

    GUI_Activity *parent = activity->parent;
    activity->parent = NULL;
    if (activity->exitCallback)
        activity->exitCallback(activity);

    current_activity = parent;

    return 0;
}

int GUI_BackToMainActivity()
{
    GUI_CloseAllDialogs(TYPE_GUI_DIALOG_ANY);

    GUI_Activity *p = current_activity;

    while (p && p->parent != NULL)
    {
        GUI_Activity *parent = p->parent;
        p->parent = NULL;
        if (p->exitCallback)
            p->exitCallback(p);
        p = parent;
    }

    current_activity = p;

    return 0;
}

int GUI_OpenDialog(GUI_Dialog *dialog)
{
    if (!dialog || current_dialog == dialog)
        return -1;

    if (current_dialog)
        current_dialog->next = dialog;
    dialog->prev = current_dialog;
    dialog->next = NULL;
    if (dialog->openCallback)
        dialog->openCallback(dialog);

    current_dialog = dialog;
    if (!main_dialog)
        main_dialog = dialog;

    return 0;
}

int GUI_CloseDialog(GUI_Dialog *dialog)
{
    if (!dialog)
        return -1;

    if (main_dialog == dialog)
        main_dialog = dialog->next;
    if (current_dialog == dialog)
        current_dialog = dialog->prev;

    if (dialog->prev)
        dialog->prev->next = dialog->next;
    if (dialog->next)
        dialog->next->prev = dialog->prev;

    dialog->prev = NULL;
    dialog->next = NULL;
    if (dialog->closeCallback)
        dialog->closeCallback(dialog);

    return 0;
}

void GUI_CloseAllDialogs(int type)
{
    GUI_Dialog *p = current_dialog;

    while (p)
    {
        GUI_Dialog *prev = p->prev;
        if (p->type & type)
            GUI_CloseDialog(p);
        p = prev;
    }
}

void GUI_CloseOtherDialogs(GUI_Dialog *dialog, int type)
{
    if (!dialog)
        return;

    GUI_Dialog *p = dialog->prev;

    while (p)
    {
        GUI_Dialog *prev = p->prev;
        if (p->type & type)
            GUI_CloseDialog(p);
        p = prev;
    }

    p = dialog->next;

    while (p)
    {
        GUI_Dialog *next = p->next;
        if (p->type & type)
            GUI_CloseDialog(p);
        p = next;
    }
}

char **GUI_GetStringArrayByIdArray(int *ids, int n_ids)
{
    if (!ids || n_ids <= 0)
        return NULL;

    char **strs = calloc(n_ids, sizeof(char *));
    if (!strs)
        return NULL;

    int i;
    for (i = 0; i < n_ids; i++)
    {
        strs[i] = cur_lang[ids[i]];
    }

    return strs;
}

void GUI_GetActivityLayoutXY(GUI_Activity *activity, int *x, int *y)
{
    *x = 0;
    if (activity->nostatusbar)
        *y = 0;
    else
        *y = STATUS_BAR_HEIGHT;
}

void GUI_GetActivityLayoutWH(GUI_Activity *activity, int *w, int *h)
{
    *w = GUI_SCREEN_WIDTH;
    if (activity->nostatusbar)
        *h = GUI_SCREEN_HEIGHT;
    else
        *h = GUI_SCREEN_HEIGHT - STATUS_BAR_HEIGHT * 2;
}

void GUI_DrawTopStatusBar(char *title)
{
    int statusbar_x = 0;
    int statusbar_y = 0;
    int statusbar_w = GUI_SCREEN_WIDTH;
    int statusbar_h = STATUS_BAR_HEIGHT;

    GUI_DrawFillRectangle(statusbar_x, statusbar_y, statusbar_w, statusbar_h, GUI_DEF_COLOR_BG);

    int x, y;
    char string[64];

    x = statusbar_x + STATUS_BAR_PADDING_L;
    y = statusbar_y + STATUS_BAR_PADDING_T;
    GUI_DrawText(x, y, COLOR_WHITE, title);

    x = statusbar_x + statusbar_w - STATUS_BAR_PADDING_L;
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
        snprintf(string, sizeof(string), "%d%%", percent);

        int battery_x = x - GUI_GetTextWidth(string);
        GUI_DrawText(battery_x, y, color, string);
        x = battery_x - STATUS_BAR_PADDING_L;
    }

    // Date & time
    SceDateTime time;
    sceRtcGetCurrentClock(&time, 0);

    char date_string[24];
    GetDateString(date_string, date_format, &time);

    char time_string[16];
    GetTimeString(time_string, time_format, &time);

    snprintf(string, sizeof(string), "%s  %s", date_string, time_string);
    int date_time_x = x - GUI_GetTextWidth(string);
    GUI_DrawText(date_time_x, y, GUI_DEF_COLOR_TEXT, string);
}

void GUI_DrawBottomStatusBar(GUI_ButtonInstruction *instructions)
{
    int statusbar_w = GUI_SCREEN_WIDTH;
    int statusbar_h = STATUS_BAR_HEIGHT;
    int statusbar_x = 0;
    int statusbar_y = GUI_SCREEN_HEIGHT - statusbar_h;

    GUI_DrawFillRectangle(statusbar_x, statusbar_y, statusbar_w, statusbar_h, GUI_DEF_COLOR_BG);

    int x = statusbar_x + STATUS_BAR_PADDING_L;
    int y = statusbar_y + STATUS_BAR_PADDING_T;

    int i;
    for (i = 0; instructions[i].button != LANG_NULL; i++)
    {
        if (!instructions[i].visibility)
            continue;
        x += GUI_DrawTextf(x, y, COLOR_AZURE, "%s:", cur_lang[instructions[i].button]);
        x += GUI_DrawText(x, y, COLOR_WHITE, cur_lang[instructions[i].instruction]);
        x += STATUS_BAR_PADDING_L;
    }
}

void GUI_DrawVerticalScrollbar(int track_x, int track_y, int track_height, int list_len, int max_draw_len, int top_pos, int draw_track)
{
    if (track_height <= 0 || max_draw_len <= 0 || list_len <= max_draw_len)
        return;

    // Draw scroll track
    if (draw_track)
        GUI_DrawFillRectangle(track_x, track_y, GUI_DEF_SCROLLBAR_SIZE, track_height, GUI_DEF_SCROLLBAR_COLOR_TRACK);

    // Draw scroll thumb
    float size_per_item = (float)track_height / (float)list_len;
    int thumb_height = (float)max_draw_len * size_per_item;
    thumb_height = MAX(thumb_height, 1); // Fix

    int min_y = track_y;
    int max_y = track_y + track_height - thumb_height;
    int thumb_y = track_y + (float)top_pos * size_per_item;
    if (thumb_y < min_y)
        thumb_y = min_y;
    else if (thumb_y > max_y)
        thumb_y = max_y;

    GUI_DrawFillRectangle(track_x, thumb_y, GUI_DEF_SCROLLBAR_SIZE, thumb_height, GUI_DEF_SCROLLBAR_COLOR_THUMB);
}

void GUI_SetWallpaperTexture(GUI_Texture *texture)
{
    GUI_WaitRenderingDone();
    if (wallpaper_texture)
        vita2d_free_texture(wallpaper_texture);
    wallpaper_texture = texture;
}

void GUI_SetSplashTexture(GUI_Texture *texture)
{
    GUI_WaitRenderingDone();
    if (splash_texture)
        vita2d_free_texture(splash_texture);
    splash_texture = texture;
}

GUI_Texture *GUI_GetDefaultWallpaper()
{
    return wallpaper_texture;
}

GUI_Texture *GUI_GetDefaultSplash()
{
    return splash_texture;
}

void GUI_DrawWallpaper(GUI_Texture *texture)
{
    if (texture)
    {
        float x_scale = (float)GUI_SCREEN_WIDTH / (float)GUI_GetTextureWidth(texture);
        float y_scale = (float)GUI_SCREEN_HEIGHT / (float)GUI_GetTextureHeight(texture);
        GUI_DrawTextureScale(texture, 0, 0, x_scale, y_scale);
    }
}

static void drawActivity()
{
    if (!current_activity)
        return;

    if (current_activity->wallpaper)
        GUI_DrawWallpaper(current_activity->wallpaper);
    else
        GUI_DrawWallpaper(wallpaper_texture);

    if (current_activity->drawCallback)
        current_activity->drawCallback(current_activity);

    if (!current_activity->nostatusbar)
    {
        GUI_DrawTopStatusBar(cur_lang[current_activity->title]);
        GUI_DrawBottomStatusBar(current_activity->button_instructions);
    }
}

static void drawDialogs()
{
    GUI_Dialog *dialog = main_dialog;

    while (dialog)
    {
        if (dialog->drawCallback)
            dialog->drawCallback(dialog);
        dialog = dialog->next;
    }
}

void GUI_DrawMain()
{
    if (Emu_IsGameLoaded())
        Emu_DrawVideo();
    else
        drawActivity();

    drawDialogs();
}

static void onHomeButtonEvent()
{
    if (released_pad[PAD_PSBUTTON])
    {
        if (!Emu_IsGameLoading() && IsPSbuttonEventEnabled())
        {
            if (!current_activity->parent)
            {
                if (current_dialog != &setting_dialog)
                {
                    GUI_OpenDialog(&setting_dialog);
                    SetPSbuttonEventEnabled(0);
                }
            }
            else
            {
                GUI_BackToMainActivity();
            }
        }
    }
}

void GUI_CtrlMain()
{
    onHomeButtonEvent();

    if (current_dialog)
    {
        if (current_dialog->ctrlCallBack)
            current_dialog->ctrlCallBack(current_dialog);
    }
    else if (current_activity)
    {
        if (current_activity->ctrlCallBack)
            current_activity->ctrlCallBack(current_activity);
    }
}

void GUI_RunMain()
{
    GUI_StartDrawing();
    GUI_DrawMain();
    GUI_EndDrawing();
    GUI_ReadPad();
    GUI_CtrlMain();
    AutoUnlockQuickMenu();
}
