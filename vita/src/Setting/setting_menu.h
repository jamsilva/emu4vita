#ifndef __M_SETTING_MENU_H__
#define __M_SETTING_MENU_H__

#include "Setting/setting.h"
#include "setting_variables.h"
#include "setting_control_options.h"
#include "setting_callbacks.h"
#include "lang.h"
#include "config.h"

enum SettingMenuListIndex
{
    MAIN_MENU_INDEX = 0,
    STATE_MENU_INDEX,
    GRAPHICS_MENU_INDEX,
    CONTROL_MENU_INDEX,
    CORE_MENU_INDEX,
    MISC_MENU_INDEX,
    APP_MENU_INDEX,
};

static char *no_yes_values[] = {
    STR_NO,
    STR_YES,
};

static char *display_size_values[] = {
    STR_DISPLAY_SIZE_1X,
    STR_DISPLAY_SIZE_2X,
    STR_DISPLAY_SIZE_3X,
    STR_DISPLAY_SIZE_FULL,
};

static char *display_ratio_values[] = {
    STR_ASPECT_RATIO_BY_REAL,
    STR_ASPECT_RATIO_BY_INFO,
    STR_ASPECT_RATIO_BY_DEV,
    STR_ASPECT_RATIO_8_7,
    STR_ASPECT_RATIO_4_3,
    STR_ASPECT_RATIO_3_2,
    STR_ASPECT_RATIO_16_9,
};

#if defined(WANT_DISPLAY_ROTATE)
static char *display_rotate_values[] = {
    STR_DISABLE,
    STR_ROTATE_90,
    STR_ROTATE_180,
    STR_ROTATE_270,
};
#endif

static char *graphics_values[] = {
    STR_DEFAULT,
    STR_SHADER_LCD3X,
    STR_SHADER_SHARP_SIMPLE,
    STR_SHADER_SHARP,
    STR_SHADER_AA,
};

static char *overlay_select_values[] = {
    STR_NONE,
};

static char *overlay_mode_values[] = {
    STR_OVERLAY_MODE_FRONT,
    STR_OVERLAY_MODE_BACK,
};

static char *map_port_values[] = {
    "1P",
    "2P",
    "3P",
    "4P",
};

static char *left_analog_values[] = {
    STR_DISABLE,
    STR_FOLLOW_DPAD,
#if 0 // defined(PS_BUILD)
	STR_BUTTON_LEFT_ANALOG,
#endif
};

static char *right_analog_values[] = {
    STR_DISABLE,
    STR_FOLLOW_DPAD,
#if 0 // defined(PS_BUILD)
	STR_BUTTON_RIGHT_ANALOG,
#endif
};

static char *preview_path_values[] = {
    STR_AUTO,
    STR_DEFAULT,
    STR_FROM_AUTO_SAVETATE,
};

static char *preview_style_values[] = {
    STR_FILL_PRESERVE,
    STR_FILL_FULL,
};

// 图形 (菜单设置)
StrArrayOption display_size_option = {&graphics_config.display_size, display_size_values, sizeof(display_size_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
StrArrayOption aspect_ratio_option = {&graphics_config.aspect_ratio, display_ratio_values, sizeof(display_ratio_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
#if defined(WANT_DISPLAY_ROTATE)
StrArrayOption display_rotate_option = {&graphics_config.display_rotate, display_rotate_values, sizeof(display_rotate_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
#endif
StrArrayOption graphics_filtering_option = {&graphics_config.graphics_filtering, graphics_values, sizeof(graphics_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
StrArrayOption smooth_graphics_option = {&graphics_config.graphics_smooth, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
StrArrayOption overlay_select_option = {&graphics_config.overlay_select, overlay_select_values, sizeof(overlay_select_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
StrArrayOption overlay_mode_option = {&graphics_config.overlay_mode, overlay_mode_values, sizeof(overlay_mode_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};
StrArrayOption show_fps_option = {&graphics_config.show_fps, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsOptionChangedCallback, NULL};

// 控制 (菜单设置)
StrArrayOption map_port_option = {&control_config.map_port, map_port_values, sizeof(map_port_values) / sizeof(char *), controlOptionChangedCallback, NULL};
OptionMenu button_left_option = STD_KEY_MAP_OPTION(0, button_left);
OptionMenu button_up_option = STD_KEY_MAP_OPTION(1, button_up);
OptionMenu button_right_option = STD_KEY_MAP_OPTION(2, button_right);
OptionMenu button_down_option = STD_KEY_MAP_OPTION(3, button_down);
OptionMenu button_cross_option = STD_KEY_MAP_OPTION(4, button_cross);
OptionMenu button_circle_option = STD_KEY_MAP_OPTION(5, button_circle);
OptionMenu button_square_option = STD_KEY_MAP_OPTION(6, button_square);
OptionMenu button_triangle_option = STD_KEY_MAP_OPTION(7, button_triangle);
OptionMenu button_l_option = STD_KEY_MAP_OPTION(8, button_l);
OptionMenu button_r_option = STD_KEY_MAP_OPTION(9, button_r);
OptionMenu button_l2_option = STD_KEY_MAP_OPTION(10, button_l2);
OptionMenu button_r2_option = STD_KEY_MAP_OPTION(11, button_r2);
OptionMenu button_l3_option = STD_KEY_MAP_OPTION(12, button_l3);
OptionMenu button_r3_option = STD_KEY_MAP_OPTION(13, button_r3);
OptionMenu button_select_option = STD_KEY_MAP_OPTION(14, button_select);
OptionMenu button_start_option = STD_KEY_MAP_OPTION(15, button_start);
StrArrayOption left_analog_option = {&control_config.left_analog, left_analog_values, sizeof(left_analog_values) / sizeof(char *), controlOptionChangedCallback, NULL};
StrArrayOption right_analog_option = {&control_config.right_analog, right_analog_values, sizeof(right_analog_values) / sizeof(char *), controlOptionChangedCallback, NULL};
StrArrayOption front_touch_pad_option = {&control_config.front_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlOptionChangedCallback, NULL};
StrArrayOption back_touch_pad_option = {&control_config.back_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlOptionChangedCallback, NULL};
IntStepOption turbo_delay_option = {&control_config.turbo_delay, 1, 30, 1, "%d", NULL, NULL};

// 杂项 (菜单设置)
StrArrayOption auto_save_load_option = {&misc_config.auto_save_load, no_yes_values, sizeof(no_yes_values) / sizeof(char *), miscOptionChangedCallback, NULL};

// 程序 (菜单设置)
StrArrayOption preview_path_option = {&app_config.preview_path, preview_path_values, sizeof(preview_path_values) / sizeof(char *), updatePreviewCallback, NULL};
StrArrayOption preview_style_option = {&app_config.preview_style, preview_style_values, sizeof(preview_style_values) / sizeof(char *), updatePreviewCallback, NULL};
StrArrayOption app_log_option = {&app_config.app_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appOptionChangedCallback, NULL};
StrArrayOption core_log_option = {&app_config.core_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appOptionChangedCallback, NULL};
StrArrayOption loading_log_option = {&app_config.loading_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appOptionChangedCallback, NULL};

// 主菜单 (菜单选项)
static SettingMenuItem main_menu_items[] = {
    {STR_RESUME_GAME, 0, OPTION_TYPE_CALLBACK, resumeGameCallback},
    {STR_RESET_GAME, 0, OPTION_TYPE_CALLBACK, resetGameCallback},
    {STR_EXIT_GAME, 0, OPTION_TYPE_CALLBACK, exitGameCallback},
    {STR_EXIT_TO_ARCH, 0, OPTION_TYPE_CALLBACK, exitToArchCallback},
    {STR_EXIT_APP, 1, OPTION_TYPE_CALLBACK, exitAppCallback},
};

// 图形 (菜单选项)
static SettingMenuItem graphics_menu_items[] = {
    {STR_DISPLAY_SIZE, 1, OPTION_TYPE_STR_ARRAY, &display_size_option},
    {STR_DISPLAY_RATIO, 1, OPTION_TYPE_STR_ARRAY, &aspect_ratio_option},
#if defined(WANT_DISPLAY_ROTATE)
    {STR_DISPLAY_ROTATE, 1, OPTION_TYPE_STR_ARRAY, &display_rotate_option},
#endif
    {STR_GRAHICS_FILTERING, 1, OPTION_TYPE_STR_ARRAY, &graphics_filtering_option},
    {STR_GRAHICS_SMOOTH, 1, OPTION_TYPE_STR_ARRAY, &smooth_graphics_option},
    {STR_OVERLAY_SELECT, 1, OPTION_TYPE_STR_ARRAY, &overlay_select_option},
    {STR_OVERLAY_MODE, 1, OPTION_TYPE_STR_ARRAY, &overlay_mode_option},
    {STR_SHOW_FPS, 1, OPTION_TYPE_STR_ARRAY, &show_fps_option},
    {STR_RESET_CONFIGS, 1, OPTION_TYPE_CALLBACK, resetGraphicsConfigCallback},
};

// 控制 (菜单选项)
SettingMenuItem control_menu_items[] = {
    //*name   enable    type    *option
    {STR_HOST_CONTROL, 1, OPTION_TYPE_STR_ARRAY, &map_port_option},
    {STR_BUTTON_LEFT, 1, OPTION_TYPE_KEY_MAP, &button_left_option},
    {STR_BUTTON_UP, 1, OPTION_TYPE_KEY_MAP, &button_up_option},
    {STR_BUTTON_RIGHT, 1, OPTION_TYPE_KEY_MAP, &button_right_option},
    {STR_BUTTON_DOWN, 1, OPTION_TYPE_KEY_MAP, &button_down_option},
    {STR_BUTTON_CROSS, 1, OPTION_TYPE_KEY_MAP, &button_cross_option},
    {STR_BUTTON_CIRCLE, 1, OPTION_TYPE_KEY_MAP, &button_circle_option},
    {STR_BUTTON_SQUARE, 1, OPTION_TYPE_KEY_MAP, &button_square_option},
    {STR_BUTTON_TRIANGLE, 1, OPTION_TYPE_KEY_MAP, &button_triangle_option},
    {STR_BUTTON_L, 1, OPTION_TYPE_KEY_MAP, &button_l_option},
    {STR_BUTTON_R, 1, OPTION_TYPE_KEY_MAP, &button_r_option},
    {STR_BUTTON_L2, 1, OPTION_TYPE_KEY_MAP, &button_l2_option},
    {STR_BUTTON_R2, 1, OPTION_TYPE_KEY_MAP, &button_r2_option},
    {STR_BUTTON_L3, 1, OPTION_TYPE_KEY_MAP, &button_l3_option},
    {STR_BUTTON_R3, 1, OPTION_TYPE_KEY_MAP, &button_r3_option},
    {STR_BUTTON_SELECT, 1, OPTION_TYPE_KEY_MAP, &button_select_option},
    {STR_BUTTON_START, 1, OPTION_TYPE_KEY_MAP, &button_start_option},
    {STR_BUTTON_LEFT_ANALOG, 1, OPTION_TYPE_STR_ARRAY, &left_analog_option},
    {STR_BUTTON_RIGHT_ANALOG, 1, OPTION_TYPE_STR_ARRAY, &right_analog_option},
    {STR_FRONT_TOUCH_TO_PAD, 1, OPTION_TYPE_STR_ARRAY, &front_touch_pad_option},
    {STR_BACK_TOUCH_TO_PAD, 1, OPTION_TYPE_STR_ARRAY, &back_touch_pad_option},
    {STR_TURBO_DELAY, 1, OPTION_TYPE_INT_STEP, &turbo_delay_option},
#if defined(WSC_BUILD)
    {STR_RESET_OPTIONS_CONTROL_H, 1, OPTION_TYPE_CALLBACK, resetControlConfigCallback},
    {STR_RESET_OPTIONS_CONTROL_V, 1, OPTION_TYPE_CALLBACK, resetVControlConfigCallback},
#else
    {STR_RESET_CONFIGS, 1, OPTION_TYPE_CALLBACK, resetControlConfigCallback},
#endif
};

// 杂项 (菜单选项)
static SettingMenuItem misc_menu_items[] = {
    {STR_AUTO_SAVE_LOAD, 1, OPTION_TYPE_STR_ARRAY, &auto_save_load_option},
    {STR_DELETE_AUTO_SAVESTATE, 1, OPTION_TYPE_CALLBACK, deleteAutoStateCallback},
    {STR_SAVE_SCREENSHOT, 0, OPTION_TYPE_CALLBACK, saveScreenshotCallback},
    {STR_SAVE_PREVIEW, 0, OPTION_TYPE_CALLBACK, saveScreenshotForPreviewCallback},
    {STR_RESET_CONFIGS, 1, OPTION_TYPE_CALLBACK, resetMiscConfigCallback},
};

// 程序 (菜单选项)
static SettingMenuItem app_menu_items[] = {
    {STR_PREVIEW_PATH, 1, OPTION_TYPE_STR_ARRAY, &preview_path_option},
    {STR_PREVIEW_STYLE, 1, OPTION_TYPE_STR_ARRAY, &preview_style_option},
    {STR_APP_LOG, 1, OPTION_TYPE_STR_ARRAY, &app_log_option},
    {STR_CORE_LOG, 1, OPTION_TYPE_STR_ARRAY, &core_log_option},
    {STR_LOADING_LOG, 1, OPTION_TYPE_STR_ARRAY, &loading_log_option},
    {STR_RESET_CONFIGS, 1, OPTION_TYPE_CALLBACK, resetAppConfigCallback},
};

// 菜单列表
static SettingMenu menu_list[] = {
    {STR_MAIN_MENU, 1, main_menu_items, sizeof(main_menu_items) / sizeof(SettingMenuItem), 0, 0, NULL, NULL},                                           // 主菜单
    {STR_STATES, 0, NULL, 0, 0, 0, stateMenuOpenCallback, stateMenuExitCallback},                                                                       // 即时存档
    {STR_GRAPHICS, 1, graphics_menu_items, sizeof(graphics_menu_items) / sizeof(SettingMenuItem), 0, 0, NULL, graphicsMenuExitCallback},                // 图形
    {STR_CONTROL, 1, control_menu_items, sizeof(control_menu_items) / sizeof(SettingMenuItem), 0, 0, NULL, controlMenuExitCallback}, // 控制
    {STR_CORE, 0, NULL, 0, 0, 0, NULL, coreMenuExitCallback},                                                                                           // 核心
    {STR_MISC, 1, misc_menu_items, sizeof(misc_menu_items) / sizeof(SettingMenuItem), 0, 0, NULL, miscMenuExitCallback},                                // 杂项
    {STR_APP, 1, app_menu_items, sizeof(app_menu_items) / sizeof(SettingMenuItem), 0, 0, NULL, appMenuExitCallback},                                    // 程序
};
#define MENU_LIST_LEN (sizeof(menu_list) / sizeof(SettingMenu))

#endif