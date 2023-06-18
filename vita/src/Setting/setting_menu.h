#ifndef __M_SETTING_MENU_H__
#define __M_SETTING_MENU_H__

#include "setting/setting.h"
#include "setting_variables.h"
#include "setting_keymapper.h"
#include "setting_callbacks.h"
#include "lang.h"
#include "config.h"

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

static int aspect_ratio_values[] = {
    DEFAULT,
    ASPECT_RATIO_BY_GAME_RESOLUTION,
    ASPECT_RATIO_BY_DEV_SCREEN,
    ASPECT_RATIO_8_7,
    ASPECT_RATIO_4_3,
    ASPECT_RATIO_3_2,
    ASPECT_RATIO_16_9,
};

#if defined(WANT_DISPLAY_ROTATE)
static int display_rotate_values[] = {
    DISABLE,
    DISPLAY_ROTATE_CW_90,
    DISPLAY_ROTATE_CW_180,
    DISPLAY_ROTATE_CW_270,
    DEFAULT,
};
#endif

static int graphics_shader_values[] = {
    DEFAULT,
    SHADER_LCD3X,
    SHADER_SHARP_BILINEAR_SIMPLE,
    SHADER_SHARP_BILINEAR,
    SHADER_ADVANCED_AA,
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
    FULL_PRESERVE,
    FULL_STRETCH,
    FULL_4_3,
    FULL_3_2,
};

// 图形 (菜单设置)
StrIndexsOption display_size_option = {&graphics_config.display_size, display_size_values, sizeof(display_size_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption aspect_ratio_option = {&graphics_config.aspect_ratio, aspect_ratio_values, sizeof(aspect_ratio_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
#if defined(WANT_DISPLAY_ROTATE)
StrIndexsOption display_rotate_option = {&graphics_config.display_rotate, display_rotate_values, sizeof(display_rotate_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
#endif
StrIndexsOption graphics_shader_option = {&graphics_config.graphics_shader, graphics_shader_values, sizeof(graphics_shader_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption graphics_smooth_option = {&graphics_config.graphics_smooth, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrArrayOption overlay_select_option = {&graphics_config.overlay_select, NULL, 0, graphicsStrArrayOptionChangedCb, NULL};
StrIndexsOption overlay_mode_option = {&graphics_config.overlay_mode, overlay_mode_values, sizeof(overlay_mode_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};
StrIndexsOption show_fps_option = {&graphics_config.show_fps, no_yes_values, sizeof(no_yes_values) / sizeof(char *), graphicsStrIndexsOptionChangedCb, NULL};

// 控制 (菜单设置)
StrArrayOption ctrl_player_option = {&control_config.ctrl_player, ctrl_player_values, sizeof(ctrl_player_values) / sizeof(char *), controlStrArrayOptionChangedCb, NULL};
KeyMapperOptionMenu button_left_option = STD_EMU_KEY_MAPPER_OPTION(0, button_left);
KeyMapperOptionMenu button_up_option = STD_EMU_KEY_MAPPER_OPTION(1, button_up);
KeyMapperOptionMenu button_right_option = STD_EMU_KEY_MAPPER_OPTION(2, button_right);
KeyMapperOptionMenu button_down_option = STD_EMU_KEY_MAPPER_OPTION(3, button_down);
KeyMapperOptionMenu button_cross_option = STD_EMU_KEY_MAPPER_OPTION(4, button_cross);
KeyMapperOptionMenu button_circle_option = STD_EMU_KEY_MAPPER_OPTION(5, button_circle);
KeyMapperOptionMenu button_square_option = STD_EMU_KEY_MAPPER_OPTION(6, button_square);
KeyMapperOptionMenu button_triangle_option = STD_EMU_KEY_MAPPER_OPTION(7, button_triangle);
KeyMapperOptionMenu button_l_option = STD_EMU_KEY_MAPPER_OPTION(8, button_l);
KeyMapperOptionMenu button_r_option = STD_EMU_KEY_MAPPER_OPTION(9, button_r);
KeyMapperOptionMenu button_l2_option = STD_EMU_KEY_MAPPER_OPTION(10, button_l2);
KeyMapperOptionMenu button_r2_option = STD_EMU_KEY_MAPPER_OPTION(11, button_r2);
KeyMapperOptionMenu button_l3_option = STD_EMU_KEY_MAPPER_OPTION(12, button_l3);
KeyMapperOptionMenu button_r3_option = STD_EMU_KEY_MAPPER_OPTION(13, button_r3);
KeyMapperOptionMenu button_select_option = STD_EMU_KEY_MAPPER_OPTION(14, button_select);
KeyMapperOptionMenu button_start_option = STD_EMU_KEY_MAPPER_OPTION(15, button_start);
StrIndexsOption left_analog_option = {&control_config.left_analog, left_analog_values, sizeof(left_analog_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption right_analog_option = {&control_config.right_analog, right_analog_values, sizeof(right_analog_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption front_touch_pad_option = {&control_config.front_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
StrIndexsOption back_touch_pad_option = {&control_config.back_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(char *), controlStrIndexsOptionChangedCb, NULL};
IntStepOption turbo_delay_option = {&control_config.turbo_delay, 1, 30, 1, "%d", controlIntStepOptionChangedCb, NULL};

// 杂项 (菜单设置)
StrIndexsOption auto_save_load_option = {&misc_config.auto_save_load, no_yes_values, sizeof(no_yes_values) / sizeof(char *), miscStrIndexsOptionChangedCb, NULL};
KeyMapperOptionMenu hk_loadstate_option = STD_HOT_KEY_MAPPER_OPTION(0, hk_loadstate);
KeyMapperOptionMenu hk_savestate_option = STD_HOT_KEY_MAPPER_OPTION(1, hk_savestate);
KeyMapperOptionMenu hk_speed_up_option = STD_HOT_KEY_MAPPER_OPTION(2, hk_speed_up);
KeyMapperOptionMenu hk_speed_down_option = STD_HOT_KEY_MAPPER_OPTION(3, hk_speed_down);
KeyMapperOptionMenu hk_player_up_option = STD_HOT_KEY_MAPPER_OPTION(4, hk_player_up);
KeyMapperOptionMenu hk_player_down_option = STD_HOT_KEY_MAPPER_OPTION(5, hk_player_down);
KeyMapperOptionMenu hk_exit_game_option = STD_HOT_KEY_MAPPER_OPTION(6, hk_exit_game);

// 程序 (菜单设置)
StrIndexsOption preview_path_option = {&app_config.preview_path, preview_path_values, sizeof(preview_path_values) / sizeof(char *), updatePreviewCallback, NULL};
StrIndexsOption preview_style_option = {&app_config.preview_style, preview_style_values, sizeof(preview_style_values) / sizeof(char *), updatePreviewCallback, NULL};
StrIndexsOption app_log_option = {&app_config.app_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrIndexsOption core_log_option = {&app_config.core_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrIndexsOption show_log_option = {&app_config.show_log, no_yes_values, sizeof(no_yes_values) / sizeof(char *), appStrIndexsOptionChangedCb, NULL};
StrArrayOption language_option = {&app_config.language, NULL, 0, updateAppLangCallback, NULL};

// 主菜单 (菜单选项)
static SettingMenuItem main_menu_items[] = {
    {LABEL_RESUME_GAME, NULL, 0, TYPE_OPTION_CALLBACK, resumeGameCallback},
    {LABEL_RESET_GAME, NULL, 0, TYPE_OPTION_CALLBACK, resetGameCallback},
    {LABEL_EXIT_GAME, NULL, 0, TYPE_OPTION_CALLBACK, exitGameCallback},
    {LABEL_DISK_CONTROL, NULL, 0, TYPE_OPTION_CALLBACK, diskControlCallback},
    {LABEL_EXIT_TO_ARCH, NULL, 0, TYPE_OPTION_CALLBACK, exitToArchCallback},
    {LABEL_EXIT_APP, NULL, 1, TYPE_OPTION_CALLBACK, exitAppCallback},
};

// 图形 (菜单选项)
static SettingMenuItem graphics_menu_items[] = {
    {LABEL_DISPLAY_SIZE, NULL, 1, TYPE_OPTION_STR_INDEXS, &display_size_option},
    {LABEL_ASPECT_RATIO, NULL, 1, TYPE_OPTION_STR_INDEXS, &aspect_ratio_option},
#if defined(WANT_DISPLAY_ROTATE)
    {LABEL_DISPLAY_ROTATE, NULL, 1, TYPE_OPTION_STR_INDEXS, &display_rotate_option},
#endif
    {LABEL_GRAHICS_SHADER, NULL, 1, TYPE_OPTION_STR_INDEXS, &graphics_shader_option},
    {LABEL_GRAHICS_SMOOTH, NULL, 1, TYPE_OPTION_STR_INDEXS, &graphics_smooth_option},
    {LABEL_OVERLAY_SELECT, NULL, 1, TYPE_OPTION_STR_ARRAY, &overlay_select_option},
    {LABEL_OVERLAY_MODE, NULL, 1, TYPE_OPTION_STR_INDEXS, &overlay_mode_option},
    {LABEL_SHOW_FPS, NULL, 1, TYPE_OPTION_STR_INDEXS, &show_fps_option},
    {LABEL_RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetGraphicsConfigCallback},
};

// 控制 (菜单选项)
SettingMenuItem control_menu_items[] = {
    //*name   enable    type    *option
    {LABEL_CTRL_PLAYER, NULL, 1, TYPE_OPTION_STR_ARRAY, &ctrl_player_option},
    {BUTTON_LEFT, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_left_option},
    {BUTTON_UP, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_up_option},
    {BUTTON_RIGHT, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_right_option},
    {BUTTON_DOWN, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_down_option},
    {BUTTON_CROSS, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_cross_option},
    {BUTTON_CIRCLE, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_circle_option},
    {BUTTON_SQUARE, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_square_option},
    {BUTTON_TRIANGLE, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_triangle_option},
    {BUTTON_L, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_l_option},
    {BUTTON_R, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_r_option},
    {BUTTON_L2, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_l2_option},
    {BUTTON_R2, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_r2_option},
    {BUTTON_L3, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_l3_option},
    {BUTTON_R3, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_r3_option},
    {BUTTON_SELECT, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_select_option},
    {BUTTON_START, NULL, 1, TYPE_OPTION_KEY_MAPPER, &button_start_option},
    {BUTTON_LEFT_ANALOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &left_analog_option},
    {BUTTON_RIGHT_ANALOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &right_analog_option},
    {LABEL_FRONT_TOUCH_TO_BUTTON, NULL, 1, TYPE_OPTION_STR_INDEXS, &front_touch_pad_option},
    {LABEL_BACK_TOUCH_TO_BUTTON, NULL, 1, TYPE_OPTION_STR_INDEXS, &back_touch_pad_option},
    {LABEL_TURBO_DELAY, NULL, 1, TYPE_OPTION_INT_STEP, &turbo_delay_option},
#if defined(WSC_BUILD)
    {LABEL_RESET_CONFIGS_H, NULL, 1, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
    {LABEL_RESET_CONFIGS_V, NULL, 1, TYPE_OPTION_CALLBACK, resetVControlConfigCallback},
#else
    {LABEL_RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
#endif
};

// 杂项 (菜单选项)
static SettingMenuItem misc_menu_items[] = {
    {LABEL_AUTO_SAVE_LOAD_STATE, NULL, 1, TYPE_OPTION_STR_INDEXS, &auto_save_load_option}, // 0
    {LABEL_HK_SAVESTATE, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_loadstate_option},          // 1
    {LABEL_HK_LOADSTATE, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_savestate_option},          // 2
    {LABEL_HK_SPEED_UP, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_speed_up_option},            // 3
    {LABEL_HK_SPEED_DOWN, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_speed_down_option},        // 4
    {LABEL_HK_PLAYER_UP, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_player_up_option},          // 5
    {LABEL_HK_PLAYER_DOWN, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_player_down_option},      // 6
    {LABEL_HK_EXIT_GAME, NULL, 1, TYPE_OPTION_KEY_MAPPER, &hk_exit_game_option},          // 7
    {LABEL_SAVE_SCREENSHOT, NULL, 0, TYPE_OPTION_CALLBACK, saveScreenshotCallback},        // 8
    {LABEL_SAVE_PREVIEW, NULL, 0, TYPE_OPTION_CALLBACK, saveScreenshotForPreviewCallback}, // 9
    {LABEL_RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetMiscConfigCallback},         // 10
};

// 程序 (菜单选项)
static SettingMenuItem app_menu_items[] = {
    {LABEL_PREVIEW_PATH, NULL, 1, TYPE_OPTION_STR_INDEXS, &preview_path_option},
    {LABEL_PREVIEW_STYLE, NULL, 1, TYPE_OPTION_STR_INDEXS, &preview_style_option},
    {LABEL_APP_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &app_log_option},
    {LABEL_CORE_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &core_log_option},
    {LABEL_SHOW_LOG, NULL, 1, TYPE_OPTION_STR_INDEXS, &show_log_option},
    {LABEL_LANGUAGE, NULL, 1, TYPE_OPTION_STR_ARRAY, &language_option},
    {LABEL_RESET_CONFIGS, NULL, 1, TYPE_OPTION_CALLBACK, resetAppConfigCallback},
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