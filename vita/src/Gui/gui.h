#ifndef __M_GUI_H__
#define __M_GUI_H__

#include "gui_lib.h"
#include "gui_ctrl.h"
#include "gui_shader.h"
#include "gui_init.h"

enum TypeGuiDialog
{
    TYPE_GUI_DIALOG_ALERT = 0x01,
    TYPE_GUI_DIALOG_WINDOW = 0x02,
    TYPE_GUI_DIALOG_ANY = 0xFF,
};

typedef struct
{
    int button;
    int instruction;
    int visible;
} GUI_ButtonInstruction;

typedef struct GUI_Activity
{
    int title;                                           // Title
    GUI_ButtonInstruction *button_instructions;          // Button instructions
    GUI_Texture *wallpaper;                              // Wallpaper
    int (*startCallback)(struct GUI_Activity *activity); // Start callback
    int (*exitCallback)(struct GUI_Activity *activity);  // Exit callback
    void (*drawCallback)(struct GUI_Activity *activity); // Draw callback
    void (*ctrlCallBack)(struct GUI_Activity *activity); // Ctrl callback
    int nostatusbar;                                     // Disable draw statusbar
    void *userdata;                                      // User data
    struct GUI_Activity *parent;                         // Parent activity
} GUI_Activity;

typedef struct GUI_Dialog
{
    int type;                                        // Dialog type
    int (*openCallback)(struct GUI_Dialog *dialog);  // Open callback
    int (*closeCallback)(struct GUI_Dialog *dialog); // Close callback
    void (*drawCallback)(struct GUI_Dialog *dialog); // Draw callback
    void (*ctrlCallBack)(struct GUI_Dialog *dialog); // Ctrl callback
    void *userdata;                                  // User data
    struct GUI_Dialog *prev;                         // Previous dialog
    struct GUI_Dialog *next;                         // Next dialog
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

// Screen
#define GUI_SCREEN_WIDTH 960
#define GUI_SCREEN_HEIGHT 544
#define GUI_SCREEN_HALF_WIDTH (GUI_SCREEN_WIDTH / 2)
#define GUI_SCREEN_HALF_HEIGHT (GUI_SCREEN_HEIGHT / 2)

// Layout
#define GUI_DEF_MAIN_LAYOUT_PADDING 10

// Scroll bar
#define GUI_DEF_SCROLLBAR_SIZE 6.0f
#define GUI_DEF_SCROLLBAR_COLOR_TRACK COLOR_ALPHA(COLOR_DARKGRAY, 0xAF)
#define GUI_DEF_SCROLLBAR_COLOR_THUMB COLOR_ALPHA(COLOR_LITEGRAY, 0x8F)

// Color
#define GUI_DEF_COLOR_TEXT COLOR_WHITE
#define GUI_DEF_COLOR_BG 0xAF1F1F1F
#define GUI_DEF_COLOR_FOCUS_BG COLOR_ALPHA(COLOR_AZURE, 0xBF)

int GUI_BackToMainActivity();
int GUI_StartActivity(GUI_Activity *activity);
int GUI_ExitActivity(GUI_Activity *activity);

int GUI_OpenDialog(GUI_Dialog *dialog);
int GUI_CloseDialog(GUI_Dialog *dialog);
void GUI_CloseAllDialogs(int type);
void GUI_CloseOtherDialogs(GUI_Dialog *dialog, int type);

void GUI_GetActivityLayoutXY(GUI_Activity *activity, int *x, int *y);
void GUI_GetActivityLayoutWH(GUI_Activity *activity, int *w, int *h);

char **GUI_GetStringArrayByIdArray(int *list, int list_len);

void GUI_DrawVerticalScrollbar(int track_x, int track_y, int track_height, int list_len, int max_draw_len, int top_pos, int draw_track);

GUI_Texture *GUI_GetDefaultWallpaper();
GUI_Texture *GUI_GetDefaultSplash();
void GUI_SetWallpaperTexture(GUI_Texture *texture);
void GUI_SetSplashTexture(GUI_Texture *texture);
void GUI_DrawWallpaper(GUI_Texture *texture);

void GUI_DrawMain();
void GUI_CtrlMain();
void GUI_RunMain();

#include "alert_dialog.h"

#endif