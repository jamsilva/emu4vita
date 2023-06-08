#ifndef __M_SETTING_VARIABLES_H__
#define __M_SETTING_VARIABLES_H__

#include "Gui/gui.h"
#include "setting.h"

// Window
#define WINDOW_WIDTH 960.0f  // 840.0f
#define WINDOW_HEIGHT 544.0f // 476.0f
#define WINDOW_PADDING 0.0f
#define WINDOW_SX (GUI_SCREEN_WIDTH - WINDOW_WIDTH) / 2
#define WINDOW_SY (GUI_SCREEN_HEIGHT - WINDOW_HEIGHT) / 2
#define WINDOW_DX (WINDOW_SX + WINDOW_WIDTH)
#define WINDOW_DY (WINDOW_SY + WINDOW_HEIGHT)
#define WINDOW_BG_COLOR COLOR_ALPHA(COLOR_BLACK, 0xAF)

#define MENU_LAYOUT_CHILD_MARGIN 8.0f

#define MENU_TAB_VIEW_PADDING_L 10.0f
#define MENU_TAB_VIEW_PADDING_T 10.0f
#define MENU_TAB_LINEVIEW_HEIGHT 3.0f

#define MENU_LISTVIEW_PADDING_L 0.0f
#define MENU_LISTVIEW_PADDING_T 8.0f

#define MENU_ITEMVIEW_PADDING_L 10.0f
#define MENU_ITEMVIEW_PADDING_T 6.0f

#define MENU_ITEMVIEW_COLOR_BG_FOCUS COLOR_ALPHA(COLOR_AZURE, 0xAF)

// Checkbox dialog
#define OPTION_LISTVIEW_WIDTH 210.0f
#define OPTION_LISTVIEW_PADDING_L 2.0f
#define OPTION_LISTVIEW_PADDING_T 2.0f
#define OPTION_ITEMVIEW_PADDING_L 10.0f
#define OPTION_ITEMVIEW_PADDING_T 6.0f

static int DialogOpen();
static int DialogClose();
static void DialogDraw();
static void DialogCtrl();

GUI_Dialog setting_dialog = {
    DialogOpen,  // Init callback
    DialogClose, // Exit callback
    DialogDraw,  // Draw callback
    DialogCtrl,  // Ctrl callback
    NULL,        // Parent dialog
};

/*
            x  x  ✓  x  ✓ ✓  ✓
    indexs: 0, 0, 0, 1, 1, 2, 3, ... (visible list index)
    list:   2, 4, 5, 6, ...          (real list index)
*/
typedef struct
{
    int list[100];        // visible list (data is real pos)
    int list_indexs[100]; // visible list indexs (get it by real pos)
    int list_len;         // visible list len
} MenuVisiblePosData;

static MenuVisiblePosData menu_visible_pos_data;

static int menu_need_refresh = 1;
static int option_display_need_refresh = 0;
static int core_menu_for_main_enabled = 0;
static int setting_config_type = 0;

static int menu_list_top_pos = 0;
static int menu_list_focus_pos = 0;
static int menu_top_pos = 0;
static int menu_focus_pos = 0;

static int option_open = 0;
static int option_top_pos = 0;
static int option_focus_pos = 0;
static OptionMenu *option_menu = NULL;

// Free draw
int SETTING_FREE_DRAW_WIDTH, SETTING_FREE_DRAW_HEIGHT;
int SETTING_FREE_DRAW_SX, SETTING_FREE_DRAW_SY, SETTING_FREE_DRAW_DX, SETTING_FREE_DRAW_DY;

// Menu
static int menu_tab_view_width, menu_tab_view_height;
static int menu_tab_view_sx, menu_tab_view_sy, menu_tab_view_dx, menu_tab_view_dy;

static int menu_listview_width, menu_listview_height;
static int menu_listview_sx, menu_listview_sy, menu_listview_dx, menu_listview_dy;
static int menu_itemview_width, menu_itemview_height;
static int menu_list_max_draw_len;
static int menu_scrollbar_track_x, menu_scrollbar_track_y, menu_scrollbar_track_height;

// Option
static int option_listview_width, option_listview_height;
static int option_listview_sx, option_listview_sy, option_listview_dx, option_listview_dy;
static int option_listview_scroll_sx;
static int option_itemview_width, option_itemview_height;
static int option_list_max_draw_len;
static int option_scrollbar_track_x, option_scrollbar_track_y, option_scrollbar_track_height;

#endif