#ifndef __M_SETTING_MENU_H__
#define __M_SETTING_MENU_H__

#include "Setting/setting.h"
#include "setting_variables.h"
#include "setting_control_options.h"
#include "setting_callbacks.h"
#include "lang.h"
#include "config.h"

enum IndexSettingMenuList
{
    INDEX_MENU_MAIN,
    INDEX_MENU_STATE,
    INDEX_MENU_GRAPHICS,
    INDEX_MENU_CONTROL,
    INDEX_MENU_CORE,
    INDEX_MENU_MISC,
    INDEX_MENU_APP,
};

static int no_yes_values[] = {
    NO,
    YES,
};

static int display_size_values[] = {
    DISPLAY_SIZE_1X,
    DISPLAY_SIZE_2X,
    DISPLAY_SIZE_3X,
    DISPLAY_SIZE_FULL,
};

static int display_ratio_values[] = {
    DEFAULT,
    DISPLAY_RATIO_BY_GAME_RESOLUTION,
    DISPLAY_RATIO_BY_DEV_SCREEN,
    DISPLAY_RATIO_8_7,
    DISPLAY_RATIO_4_3,
    DISPLAY_RATIO_3_2,
    DISPLAY_RATIO_16_9,
};

#if defined(WANT_DISPLAY_ROTATE)
static int display_rotate_values[] = {
    DISABLE,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270,
};
#endif

static int graphics_values[] = {
    DEFAULT,
    SHADER_LCD3X,
    SHADER_SHARP_SIMPLE,
    SHADER_SHARP,
    SHADER_AA,
};

static int overlay_mode_values[] = {
    OVERLAY_MODE_OVERLAY,
    OVERLAY_MODE_BACKGROUND,
};

static char *ctrl_player_values[] = {
    "1P",
    "2P",
    "3P",
    "4P",
};

static int left_analog_values[] = {
    DISABLE,
    FOLLOW_DPAD,
};

static int right_analog_values[] = {
    DISABLE,
    FOLLOW_DPAD,
};

static int preview_path_values[] = {
    AUTO,
    DEFAULT,
    FROM_AUTO_STATE,
};

static int preview_style_values[] = {
    PRESERVE_FULL,
    STRETCH_FULL,
};

// 图形 (菜单设置)
StrIndexsOption display_size_option = {&graphics_config.display_size, display_size_values, sizeof(display_size_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption aspect_ratio_option = {&graphics_config.aspect_ratio, display_ratio_values, sizeof(display_ratio_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
#if defined(WANT_DISPLAY_ROTATE)
StrIndexsOption display_rotate_option = {&graphics_config.display_rotate, display_rotate_values, sizeof(display_rotate_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
#endif
StrIndexsOption graphics_filtering_option = {&graphics_config.graphics_filtering, graphics_values, sizeof(graphics_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption smooth_graphics_option = {&graphics_config.graphics_smooth, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrArrayOption overlay_select_option = {&graphics_config.overlay_select, NULL, 0, graphicsStrArrayOptionChangedCb, NULL};
StrIndexsOption overlay_mode_option = {&graphics_config.overlay_mode, overlay_mode_values, sizeof(overlay_mode_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption show_fps_option = {&graphics_config.show_fps, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};

// 控制 (菜单设置)
StrArrayOption ctrl_player_option = {&control_config.map_port, ctrl_player_values, sizeof(ctrl_player_values) / sizeof(char *), controlStrArrayOptionChangedCb, NULL};
KeyMapOptionMenu button_left_option = STD_KEY_MAP_OPTION(0, button_left);
KeyMapOptionMenu button_up_option = STD_KEY_MAP_OPTION(1, button_up);
KeyMapOptionMenu button_right_option = STD_KEY_MAP_OPTION(2, button_right);
KeyMapOptionMenu button_down_option = STD_KEY_MAP_OPTION(3, button_down);
KeyMapOptionMenu button_cross_option = STD_KEY_MAP_OPTION(4, button_cross);
KeyMapOptionMenu button_circle_option = STD_KEY_MAP_OPTION(5, button_circle);
KeyMapOptionMenu button_square_option = STD_KEY_MAP_OPTION(6, button_square);
KeyMapOptionMenu button_triangle_option = STD_KEY_MAP_OPTION(7, button_triangle);
KeyMapOptionMenu button_l_option = STD_KEY_MAP_OPTION(8, button_l);
KeyMapOptionMenu button_r_option = STD_KEY_MAP_OPTION(9, button_r);
KeyMapOptionMenu button_l2_option = STD_KEY_MAP_OPTION(10, button_l2);
KeyMapOptionMenu button_r2_option = STD_KEY_MAP_OPTION(11, button_r2);
KeyMapOptionMenu button_l3_option = STD_KEY_MAP_OPTION(12, button_l3);
KeyMapOptionMenu button_r3_option = STD_KEY_MAP_OPTION(13, button_r3);
KeyMapOptionMenu button_select_option = STD_KEY_MAP_OPTION(14, button_select);
KeyMapOptionMenu button_start_option = STD_KEY_MAP_OPTION(15, button_start);
StrIndexsOption left_analog_option = {&control_config.left_analog, left_analog_values, sizeof(left_analog_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption right_analog_option = {&control_config.right_analog, right_analog_values, sizeof(right_analog_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption front_touch_pad_option = {&control_config.front_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption back_touch_pad_option = {&control_config.back_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
IntStepOption turbo_delay_option = {&control_config.turbo_delay, 1, 30, 1, "%d", controlIntStepOptionChangedCb, NULL};

// 杂项 (菜单设置)
StrIndexsOption auto_save_load_option = {&misc_config.auto_save_load, no_yes_values, sizeof(no_yes_values) / sizeof(char *), miscStrIndexsOptionChangedCb, NULL};

// 程序 (菜单设置)
StrIndexsOption preview_path_option = {&app_config.preview_path, preview_path_values, sizeof(preview_path_values) / sizeof(char *), updatePreviewCallback, NULL};
StrIndexsOption preview_style_option = {&app_config.preview_style, preview_style_values, sizeof(preview_style_values) / sizeof(char *), updatePreviewCallback, NULL};
StrIndexsOption app_log_option = {&app_config.app_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrIndexsOption core_log_option = {&app_config.core_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrIndexsOption print_log_option = {&app_config.print_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrArrayOption app_lang_option = {&app_config.app_lang, NULL, 0, updateAppLangCallback, NULL};

// 主菜单 (菜单选项)
static SettingMenuItem main_menu_items[] = {
    {RESUME_GAME, NULL, 0, TYPE_OPTION_CALLBACK, resumeGameCallback},
    {RESET_GAME, NULL, 0, TYPE_OPTION_CALLBACK, resetGameCallback},
    {EXIT_GAME, NULL, 0, TYPE_OPTION_CALLBACK, exitGameCallback},
    {EXIT_TO_ARCH, NULL, 0, TYPE_OPTION_CALLBACK, exitToArchCallback},
    {EXIT_APP, NULL, 1, TYPE_OPTION_CALLBACK, exitAppCallback},
};

// 图形 (菜单选项)
static SettingMenuItem graphics_menu_items[] = {
    {DISPLAY_SIZE, NULL, 1, TYPE_OPTION_STR_INDEXS, &display_size_option},
    {DISPLAY_RATIO, NULL, 1, TYPE_OPTION_STR_INDEXS, &aspect_ratio_option},
#if defined(WANT_DISPLAY_ROTATE)
    {DISPLAY_ROTATE, NULL, 1, TYPE_OPTION_STR_INDEXS, &display_rotate_option},
#endif
    {GRAHICS_FILTERING, NULL, 1, TYPE_OPTION_STR_INDEXS, &graphics_filtering_option},
    {GRAHICS_SMOOTH, NULL, 1, TYPE_OPTION_STR_INDEXS, &smooth_graphics_option},
    {OVERLAY_SELECT, NULL, 1, TYPE_OPTION_STR_ARRAY, &overlay_select_option},
    {OVERLAY_MODE, NULL, 1, TYPE_OPTION_STR_INDEXS, &overlay_mode_option},
    {SHOW_FPS, NULL, 1, TYPE_OPTION_STR_INDEXS, &show_fps_option},
    {RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetGraphicsConfigCallback},
};

// 控制 (菜单选项)
SettingMenuItem control_menu_items[] = {
    //*name   enable    type    *option
    {CTRL_PLAYER, NULL, 1, TYPE_OPTION_STR_ARRAY, &ctrl_player_option},
    {BUTTON_LEFT, NULL, 1, TYPE_OPTION_KEY_MAP, &button_left_option},
    {BUTTON_UP, NULL, 1, TYPE_OPTION_KEY_MAP, &button_up_option},
    {BUTTON_RIGHT, NULL, 1, TYPE_OPTION_KEY_MAP, &button_right_option},
    {BUTTON_DOWN, NULL, 1, TYPE_OPTION_KEY_MAP, &button_down_option},
    {BUTTON_CROSS, NULL, 1, TYPE_OPTION_KEY_MAP, &button_cross_option},
    {BUTTON_CIRCLE, NULL, 1, TYPE_OPTION_KEY_MAP, &button_circle_option},
    {BUTTON_SQUARE, NULL, 1, TYPE_OPTION_KEY_MAP, &button_square_option},
    {BUTTON_TRIANGLE, NULL, 1, TYPE_OPTION_KEY_MAP, &button_triangle_option},
    {BUTTON_L, NULL, 1, TYPE_OPTION_KEY_MAP, &button_l_option},
    {BUTTON_R, NULL, 1, TYPE_OPTION_KEY_MAP, &button_r_option},
    {BUTTON_L2, NULL, 1, TYPE_OPTION_KEY_MAP, &button_l2_option},
    {BUTTON_R2, NULL, 1, TYPE_OPTION_KEY_MAP, &button_r2_option},
    {BUTTON_L3, NULL, 1, TYPE_OPTION_KEY_MAP, &button_l3_option},
    {BUTTON_R3, NULL, 1, TYPE_OPTION_KEY_MAP, &button_r3_option},
    {BUTTON_SELECT, NULL, 1, TYPE_OPTION_KEY_MAP, &button_select_option},
    {BUTTON_START, NULL, 1, TYPE_OPTION_KEY_MAP, &button_start_option},
    {BUTTON_LEFT_ANALOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &left_analog_option},
    {BUTTON_RIGHT_ANALOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &right_analog_option},
    {FRONT_TOUCH_MAP_CTRL, NULL, 1, TYPE_OPTION_STR_INDEXS, &front_touch_pad_option},
    {BACK_TOUCH_MAP_CTRL, NULL, 1, TYPE_OPTION_STR_INDEXS, &back_touch_pad_option},
    {TURBO_DELAY, NULL, 1, TYPE_OPTION_INT_STEP, &turbo_delay_option},
#if defined(WSC_BUILD)
    {RESET_CTRL_CONFIGS_H, NULL, 1, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
    {RESET_CTRL_CONFIGS_V, NULL, 1, TYPE_OPTION_CALLBACK, resetVControlConfigCallback},
#else
    {RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
#endif
};

// 杂项 (菜单选项)
static SettingMenuItem misc_menu_items[] = {
    {AUTO_SAVE_LOAD_STATE, NULL, 1, TYPE_OPTION_STR_INDEXS, &auto_save_load_option},
    {SAVE_SCREENSHOT, NULL, 0, TYPE_OPTION_CALLBACK, saveScreenshotCallback},
    {SAVE_PREVIEW, NULL, 0, TYPE_OPTION_CALLBACK, saveScreenshotForPreviewCallback},
    {RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetMiscConfigCallback},
};

// 程序 (菜单选项)
static SettingMenuItem app_menu_items[] = {
    {PREVIEW_PATH, NULL, 1, TYPE_OPTION_STR_INDEXS, &preview_path_option},
    {PREVIEW_STYLE, NULL, 1, TYPE_OPTION_STR_INDEXS, &preview_style_option},
    {APP_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &app_log_option},
    {CORE_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &core_log_option},
    {PRINT_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &print_log_option},
    {LANGUAGE, NULL, 1, TYPE_OPTION_STR_ARRAY, &app_lang_option},
    {RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetAppConfigCallback},
};

// 菜单列表
static SettingMenu menu_list[] = {
    {TAB_MAIN, 1, main_menu_items, sizeof(main_menu_items) / sizeof(SettingMenuItem), NULL, NULL},                                 // 主菜单
    {TAB_STATE, 0, NULL, 0, NULL, stateMenuExitCallback},                                                                          // 即时存档
    {TAB_GRAPHICS, 1, graphics_menu_items, sizeof(graphics_menu_items) / sizeof(SettingMenuItem), NULL, graphicsMenuExitCallback}, // 图形
    {TAB_CONTROL, 1, control_menu_items, sizeof(control_menu_items) / sizeof(SettingMenuItem), NULL, controlMenuExitCallback},     // 控制
    {TAB_CORE, 0, NULL, 0, NULL, coreMenuExitCallback},                                                                            // 核心
    {TAB_MISC, 1, misc_menu_items, sizeof(misc_menu_items) / sizeof(SettingMenuItem), NULL, miscMenuExitCallback},                 // 杂项
    {TAB_APP, 1, app_menu_items, sizeof(app_menu_items) / sizeof(SettingMenuItem), NULL, appMenuExitCallback},                     // 程序
};
#define MENU_LIST_LEN (sizeof(menu_list) / sizeof(SettingMenu))

#endif