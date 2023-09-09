#include "gui/gui.h"
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

// Checkbox dialog
#define OPTION_LISTVIEW_WIDTH 230.0f
#define OPTION_LISTVIEW_PADDING_L 2.0f
#define OPTION_LISTVIEW_PADDING_T 2.0f
#define OPTION_ITEMVIEW_PADDING_L 10.0f
#define OPTION_ITEMVIEW_PADDING_T 6.0f

#define MENU_ITEMVIEW_COLOR_FOCUS_BG GUI_DEF_COLOR_FOCUS_BG

enum IndexSettingMenuList
{
    INDEX_MENU_MAIN,
    INDEX_MENU_STATE,
    INDEX_MENU_GRAPHICS,
    INDEX_MENU_CONTROL,
    INDEX_MENU_HOTKEY,
    INDEX_MENU_CORE,
    INDEX_MENU_MISC,
    INDEX_MENU_APP,
};

extern GUI_Dialog setting_dialog;

static int menu_need_update = 1;
static int menu_items_need_update = 0;
static int has_main_core_menu = 0;
static int setting_config_type = 0;

// static int menu_list_top_pos = 0;
static int menu_list_focus_pos = 0;
static int menu_top_pos = 0;
static int menu_focus_pos = 0;

static int option_open = 0;
static int option_top_pos = 0;
static int option_focus_pos = 0;
static CheckBoxOptionMenu *option_menu = NULL;

static int app_option_changed = 0;
static int graphics_option_changed = 0;
static int control_option_changed = 0;
static int hotkey_option_changed = 0;
static int core_option_changed = 0;
static int misc_option_changed = 0;

static int current_path_is_file = 0;
static int game_is_loaded = 0;

static int visibility_visible = 1;
// static int visibility_invisible = 0;
static int core_menu_visibility = 0;
static int app_menu_visibility = 0;
static int disk_control_visibility = 0;
static int exit_to_arch_visibility = 0;
static int touch_to_button_visibility = 0;

static SettingMenuItem **select_menu_items = NULL;
static int n_select_menu_items = 0;

// Free draw
int SETTING_FREE_DRAW_WIDTH, SETTING_FREE_DRAW_HEIGHT;
int SETTING_FREE_DRAW_SX, SETTING_FREE_DRAW_SY, SETTING_FREE_DRAW_DX, SETTING_FREE_DRAW_DY;

// Menu
static int menu_tab_view_width, menu_tab_view_height;
static int menu_tab_view_sx, menu_tab_view_sy, menu_tab_view_dx, menu_tab_view_dy;

static int menu_listview_width, menu_listview_height;
static int menu_listview_sx, menu_listview_sy, menu_listview_dx, menu_listview_dy;
static int menu_itemview_width, menu_itemview_height;
static int menu_listview_n_draw_items;
static int menu_scrollbar_track_x, menu_scrollbar_track_y, menu_scrollbar_track_height;

// Option
static int option_listview_width, option_listview_height;
static int option_listview_sx, option_listview_sy, option_listview_dx, option_listview_dy;
static int option_listview_scroll_sx;
static int option_itemview_width, option_itemview_height;
static int option_listview_n_draw_items;
static int option_scrollbar_track_x, option_scrollbar_track_y, option_scrollbar_track_height;
