#ifndef __M_GUI_H__
#define __M_GUI_H__

#include "gui_lib.h"
#include "gui_ctrl.h"
#include "gui_shader.h"
#include "gui_init.h"

typedef struct
{
    int button;
    int instruction;
} GUI_ButtonInstruction;

typedef struct GUI_Activity
{
    int title;                                // Title
    GUI_ButtonInstruction *button_instructions; // Button instructions
    int (*init)();                              // Init callback
    int (*exit)();                              // Exit callback
    void (*draw)();                             // Draw callback
    void (*ctrl)();                             // Ctrl callback
    int nostatusbar;                            // Disable draw statusbar
    int nowallpaper;                            // Disable draw wallpaper
    struct GUI_Activity *parent;                // Parent activity
} GUI_Activity;

typedef struct GUI_Dialog
{
    int (*open)();             // Init callback
    int (*close)();            // Exit callback
    void (*draw)();            // Draw callback
    void (*ctrl)();            // Ctrl callback
    struct GUI_Dialog *parent; // Parent dialog
} GUI_Dialog;

// Primary colors
#define COLOR_RED 0xFF0000FF
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE 0xFFFF0000
// Secondary colors
#define COLOR_CYAN 0xFFFFFF00
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_YELLOW 0xFF00FFFF
// Tertiary colors
#define COLOR_AZURE 0xFFFF7F00
#define COLOR_VIOLET 0xFFFF007F
#define COLOR_ROSE 0xFF7F00FF
#define COLOR_ORANGE 0xFF007FFF
#define COLOR_CHARTREUSE 0xFF00FF7F
#define COLOR_SPRING_GREEN 0xFF7FFF00
// Grayscale
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_LITEGRAY 0xFFBFBFBF
#define COLOR_GRAY 0xFF7F7F7F
#define COLOR_DARKGRAY 0xFF3F3F3F
#define COLOR_BLACK 0xFF000000

#define NOALPHA 0xFF
#define COLOR_ALPHA(color, alpha) (color & 0x00FFFFFF) | ((alpha & 0xFF) << 24)

// Define colors
#define GUI_DEFALUT_TEXT_COLOR COLOR_WHITE
#define GUI_DEFALUT_BG_COLOR 0xAF1F1F1F

// Screen size
#define GUI_SCREEN_WIDTH 960.0f
#define GUI_SCREEN_HEIGHT 544.0f
#define GUI_SCREEN_HALF_WIDTH (GUI_SCREEN_WIDTH / 2)
#define GUI_SCREEN_HALF_HEIGHT (GUI_SCREEN_HEIGHT / 2)

// Scroll bar
#define GUI_SCROLLBAR_SIZE 6.0f
#define GUI_SCROLLBAR_TRACK_COLOR COLOR_ALPHA(COLOR_DARKGRAY, 0xAF)
#define GUI_SCROLLBAR_THUMB_COLOR COLOR_ALPHA(COLOR_LITEGRAY, 0x8F)

#define BUTTON_ENTER -1
#define BUTTON_CANCEL -2
#define APP_TITLE -3
#define LANG_DISABLE -1000

// Free draw
extern float ACTIVITY_FREE_DRAW_WIDTH, ACTIVITY_FREE_DRAW_HEIGHT;
extern float ACTIVITY_FREE_DRAW_SX, ACTIVITY_FREE_DRAW_SY, ACTIVITY_FREE_DRAW_DX, ACTIVITY_FREE_DRAW_DY;

void GUI_RefreshLayout();
void GUI_GetEnterButton();

int GUI_SetMainActivity(GUI_Activity *activity);
int GUI_BacktoMainActivity();
int GUI_EnterActivity(GUI_Activity *activity);
int GUI_ExitActivity(GUI_Activity *activity);

int GUI_OpenDialog(GUI_Dialog *activity);
int GUI_CloseDialog(GUI_Dialog *activity);

void GUI_DrawVerticalScrollbar(int track_sx, int track_sy, int track_height, int list_len, int max_draw_len, int top_pos, int draw_track);

void GUI_SetWallpaperTexture(GUI_Texture *texture);
void GUI_SetSplashTexture(GUI_Texture *texture);

void GUI_DisplaySafeMode();
void GUI_DrawWallpaper();
void GUI_DrawSplash();
void GUI_DisplaySplash();

void GUI_DrawMain();
void GUI_CtrlMain();
void GUI_RunMain();

#endif