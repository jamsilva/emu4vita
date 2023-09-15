#include "setting.h"
#include "lang.h"
#include "config.h"

#ifndef __SETTING_MENU_EMBED
#define __SETTING_MENU_EMBED
#endif
#include "menu_keymapper.c"
#include "menu_callbacks.c"

static LangString no_yes_values[] = {
    {NO, NULL},
    {YES, NULL},
};

static LangString display_size_values[] = {
    {DISPLAY_SIZE_1X, NULL},
    {DISPLAY_SIZE_2X, NULL},
    {DISPLAY_SIZE_3X, NULL},
    {DISPLAY_SIZE_FULL, NULL},
};

static LangString aspect_ratio_values[] = {
    {DEFAULT, NULL},
    {ASPECT_RATIO_BY_GAME_RESOLUTION, NULL},
    {ASPECT_RATIO_BY_DEV_SCREEN, NULL},
    {ASPECT_RATIO_8_7, NULL},
    {ASPECT_RATIO_4_3, NULL},
    {ASPECT_RATIO_3_2, NULL},
    {ASPECT_RATIO_16_9, NULL},
};

#if defined(WANT_DISPLAY_ROTATE)
static LangString display_rotate_values[] = {
    {DISABLE, NULL},
    {DISPLAY_ROTATE_CW_90, NULL},
    {DISPLAY_ROTATE_CW_180, NULL},
    {DISPLAY_ROTATE_CW_270, NULL},
    {DEFAULT, NULL},
};
#endif

static LangString graphics_shader_values[] = {
    {DEFAULT, NULL},
    {SHADER_LCD3X, NULL},
    {SHADER_SHARP_BILINEAR_SIMPLE, NULL},
    {SHADER_SHARP_BILINEAR, NULL},
    {SHADER_ADVANCED_AA, NULL},
};

static LangString overlay_mode_values[] = {
    {OVERLAY_MODE_OVERLAY, NULL},
    {OVERLAY_MODE_BACKGROUND, NULL},
};

static LangString ctrl_player_values[] = {
    {LANG_NULL, "1P"},
    {LANG_NULL, "2P"},
    {LANG_NULL, "3P"},
    {LANG_NULL, "4P"},
};

static LangString preview_path_values[] = {
    {AUTO, NULL},
    {DEFAULT, NULL},
    {FROM_AUTO_STATE, NULL},
};

static LangString preview_style_values[] = {
    {FULL_PRESERVE, NULL},
    {FULL_STRETCH, NULL},
    {FULL_CROP, NULL},
};

// 图形 (菜单条目)
StrArrayOption display_size_option = {&graphics_config.display_size, display_size_values, sizeof(display_size_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
StrArrayOption aspect_ratio_option = {&graphics_config.aspect_ratio, aspect_ratio_values, sizeof(aspect_ratio_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
#if defined(WANT_DISPLAY_ROTATE)
StrArrayOption display_rotate_option = {&graphics_config.display_rotate, display_rotate_values, sizeof(display_rotate_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
#endif
StrArrayOption graphics_shader_option = {&graphics_config.graphics_shader, graphics_shader_values, sizeof(graphics_shader_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
StrArrayOption graphics_smooth_option = {&graphics_config.graphics_smooth, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
StrArrayOption overlay_select_option = {&graphics_config.overlay_select, NULL, 0, graphicsOptionUpdateCallback, NULL};
StrArrayOption overlay_mode_option = {&graphics_config.overlay_mode, overlay_mode_values, sizeof(overlay_mode_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};
StrArrayOption show_fps_option = {&graphics_config.show_fps, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), graphicsOptionUpdateCallback, NULL};

// 控制 (菜单条目)
StrArrayOption ctrl_player_option = {&control_config.ctrl_player, ctrl_player_values, sizeof(ctrl_player_values) / sizeof(LangString), (void *)controlOptionUpdateCallback, NULL};
CheckBoxOptionMenu button_left_option = STD_EMU_KEY_MAPPER_OPTION(0, button_left);
CheckBoxOptionMenu button_up_option = STD_EMU_KEY_MAPPER_OPTION(1, button_up);
CheckBoxOptionMenu button_right_option = STD_EMU_KEY_MAPPER_OPTION(2, button_right);
CheckBoxOptionMenu button_down_option = STD_EMU_KEY_MAPPER_OPTION(3, button_down);
CheckBoxOptionMenu button_cross_option = STD_EMU_KEY_MAPPER_OPTION(4, button_cross);
CheckBoxOptionMenu button_circle_option = STD_EMU_KEY_MAPPER_OPTION(5, button_circle);
CheckBoxOptionMenu button_square_option = STD_EMU_KEY_MAPPER_OPTION(6, button_square);
CheckBoxOptionMenu button_triangle_option = STD_EMU_KEY_MAPPER_OPTION(7, button_triangle);
CheckBoxOptionMenu button_l_option = STD_EMU_KEY_MAPPER_OPTION(8, button_l);
CheckBoxOptionMenu button_r_option = STD_EMU_KEY_MAPPER_OPTION(9, button_r);
CheckBoxOptionMenu button_l2_option = STD_EMU_KEY_MAPPER_OPTION(10, button_l2);
CheckBoxOptionMenu button_r2_option = STD_EMU_KEY_MAPPER_OPTION(11, button_r2);
CheckBoxOptionMenu button_l3_option = STD_EMU_KEY_MAPPER_OPTION(12, button_l3);
CheckBoxOptionMenu button_r3_option = STD_EMU_KEY_MAPPER_OPTION(13, button_r3);
CheckBoxOptionMenu button_select_option = STD_EMU_KEY_MAPPER_OPTION(14, button_select);
CheckBoxOptionMenu button_start_option = STD_EMU_KEY_MAPPER_OPTION(15, button_start);
CheckBoxOptionMenu left_analog_left_option = STD_EMU_KEY_MAPPER_OPTION(16, left_analog_left);
CheckBoxOptionMenu left_analog_up_option = STD_EMU_KEY_MAPPER_OPTION(17, left_analog_up);
CheckBoxOptionMenu left_analog_right_option = STD_EMU_KEY_MAPPER_OPTION(18, left_analog_right);
CheckBoxOptionMenu left_analog_down_option = STD_EMU_KEY_MAPPER_OPTION(19, left_analog_down);
CheckBoxOptionMenu right_analog_left_option = STD_EMU_KEY_MAPPER_OPTION(20, right_analog_left);
CheckBoxOptionMenu right_analog_up_option = STD_EMU_KEY_MAPPER_OPTION(21, right_analog_up);
CheckBoxOptionMenu right_analog_right_option = STD_EMU_KEY_MAPPER_OPTION(22, right_analog_right);
CheckBoxOptionMenu right_analog_down_option = STD_EMU_KEY_MAPPER_OPTION(23, right_analog_down);
StrArrayOption front_touch_pad_option = {&control_config.front_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), (void *)controlOptionUpdateCallback, NULL};
StrArrayOption back_touch_pad_option = {&control_config.back_touch_pad, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), (void *)controlOptionUpdateCallback, NULL};
IntRangeOption turbo_delay_option = {&control_config.turbo_delay, 1, 30, 1, "%d", (void *)controlOptionUpdateCallback, NULL};

// 热键 (菜单条目)
CheckBoxOptionMenu hk_loadstate_option = STD_HOT_KEY_MAPPER_OPTION(0, hk_loadstate);
CheckBoxOptionMenu hk_savestate_option = STD_HOT_KEY_MAPPER_OPTION(1, hk_savestate);
CheckBoxOptionMenu hk_speed_up_option = STD_HOT_KEY_MAPPER_OPTION(2, hk_speed_up);
CheckBoxOptionMenu hk_speed_down_option = STD_HOT_KEY_MAPPER_OPTION(3, hk_speed_down);
CheckBoxOptionMenu hk_player_up_option = STD_HOT_KEY_MAPPER_OPTION(4, hk_player_up);
CheckBoxOptionMenu hk_player_down_option = STD_HOT_KEY_MAPPER_OPTION(5, hk_player_down);
CheckBoxOptionMenu hk_exit_game_option = STD_HOT_KEY_MAPPER_OPTION(6, hk_exit_game);

// 杂项 (菜单条目)
StrArrayOption auto_save_load_option = {&misc_config.auto_save_load, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), miscOptionUpdateCallback, NULL};

// 程序 (菜单条目)
StrArrayOption preview_path_option = {&app_config.preview_path, preview_path_values, sizeof(preview_path_values) / sizeof(LangString), previewOptionUpdateCallback, NULL};
StrArrayOption preview_style_option = {&app_config.preview_style, preview_style_values, sizeof(preview_style_values) / sizeof(LangString), previewOptionUpdateCallback, NULL};
StrArrayOption app_log_option = {&app_config.app_log, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), appOptionUpdateCallback, NULL};
StrArrayOption core_log_option = {&app_config.core_log, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), appOptionUpdateCallback, NULL};
StrArrayOption show_log_option = {&app_config.show_log, no_yes_values, sizeof(no_yes_values) / sizeof(LangString), appOptionUpdateCallback, NULL};
StrArrayOption language_option = {&app_config.language, NULL, 0, langOptionUpdateCallback, NULL};

// 主菜单
static SettingMenuItem main_menu_items[] = {
    {{LABEL_RESUME_GAME, NULL}, &game_is_loaded, TYPE_OPTION_CALLBACK, resumeGameCallback},
    {{LABEL_RESET_GAME, NULL}, &game_is_loaded, TYPE_OPTION_CALLBACK, resetGameCallback},
    {{LABEL_EXIT_GAME, NULL}, &game_is_loaded, TYPE_OPTION_CALLBACK, exitGameCallback},
    {{LABEL_DISK_CONTROL, NULL}, &disk_control_visibility, TYPE_OPTION_CALLBACK, diskControlCallback},
    {{LABEL_EXIT_TO_ARCH, NULL}, &exit_to_arch_visibility, TYPE_OPTION_CALLBACK, exitToArchCallback},
    {{LABEL_EXIT_APP, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, exitAppCallback},
};
static SettingMenu main_menu = {{TAB_MAIN, NULL}, &visibility_visible, main_menu_items, sizeof(main_menu_items) / sizeof(SettingMenuItem), NULL, NULL};

// 即时存档
static SettingMenu state_menu = {{TAB_STATE, NULL}, &current_path_is_file, NULL, 0, stateMenuEnterCallback, stateMenuExitCallback};

// 图形
static SettingMenuItem graphics_menu_items[] = {
    {{LABEL_DISPLAY_SIZE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &display_size_option},
    {{LABEL_ASPECT_RATIO, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &aspect_ratio_option},
#if defined(WANT_DISPLAY_ROTATE)
    {{LABEL_DISPLAY_ROTATE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &display_rotate_option},
#endif
    {{LABEL_GRAHICS_SHADER, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &graphics_shader_option},
    {{LABEL_GRAHICS_SMOOTH, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &graphics_smooth_option},
    {{LABEL_OVERLAY_SELECT, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &overlay_select_option},
    {{LABEL_OVERLAY_MODE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &overlay_mode_option},
    {{LABEL_SHOW_FPS, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &show_fps_option},
    {{LABEL_RESET_CONFIGS, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetGraphicsConfigCallback},
};
static SettingMenu graphics_menu = {{TAB_GRAPHICS, NULL}, &visibility_visible, graphics_menu_items, sizeof(graphics_menu_items) / sizeof(SettingMenuItem), NULL, graphicsMenuExitCallback};

// 控制 (菜单选项)
SettingMenuItem control_menu_items[] = {
    {{LABEL_CTRL_PLAYER, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &ctrl_player_option},
    {{BUTTON_LEFT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_left_option},
    {{BUTTON_UP, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_up_option},
    {{BUTTON_RIGHT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_right_option},
    {{BUTTON_DOWN, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_down_option},
    {{BUTTON_CROSS, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_cross_option},
    {{BUTTON_CIRCLE, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_circle_option},
    {{BUTTON_SQUARE, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_square_option},
    {{BUTTON_TRIANGLE, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_triangle_option},
    {{BUTTON_L, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_l_option},
    {{BUTTON_R, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_r_option},
    {{BUTTON_L2, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_l2_option},
    {{BUTTON_R2, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_r2_option},
    {{BUTTON_L3, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_l3_option},
    {{BUTTON_R3, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_r3_option},
    {{BUTTON_SELECT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_select_option},
    {{BUTTON_START, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &button_start_option},
    
    {{BUTTON_LEFT_ANALOG_LEFT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &left_analog_left_option},
    {{BUTTON_LEFT_ANALOG_UP, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &left_analog_up_option},
    {{BUTTON_LEFT_ANALOG_RIGHT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &left_analog_right_option},
    {{BUTTON_LEFT_ANALOG_DOWN, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &left_analog_down_option},
    {{BUTTON_RIGHT_ANALOG_LEFT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &right_analog_left_option},
    {{BUTTON_RIGHT_ANALOG_UP, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &right_analog_up_option},
    {{BUTTON_RIGHT_ANALOG_RIGHT, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &right_analog_right_option},
    {{BUTTON_RIGHT_ANALOG_DOWN, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &right_analog_down_option},


    {{LABEL_FRONT_TOUCH_TO_BUTTON, NULL}, &touch_to_button_visibility, TYPE_OPTION_STR_ARRAY, &front_touch_pad_option},
    {{LABEL_BACK_TOUCH_TO_BUTTON, NULL}, &touch_to_button_visibility, TYPE_OPTION_STR_ARRAY, &back_touch_pad_option},
    {{LABEL_TURBO_DELAY, NULL}, &visibility_visible, TYPE_OPTION_INT_RANGE, &turbo_delay_option},
#if defined(WSC_BUILD)
    {{LABEL_RESET_CONFIGS_H, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
    {{LABEL_RESET_CONFIGS_V, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetVControlConfigCallback},
#else
    {{LABEL_RESET_CONFIGS, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetControlConfigCallback},
#endif
};
static SettingMenu control_menu = {{TAB_CONTROL, NULL}, &visibility_visible, control_menu_items, sizeof(control_menu_items) / sizeof(SettingMenuItem), NULL, controlMenuExitCallback};

// 热键
static SettingMenuItem hotkey_menu_items[] = {
    {{LABEL_HK_SAVESTATE, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_loadstate_option},      // 0
    {{LABEL_HK_LOADSTATE, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_savestate_option},      // 1
    {{LABEL_HK_SPEED_UP, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_speed_up_option},        // 2
    {{LABEL_HK_SPEED_DOWN, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_speed_down_option},    // 3
    {{LABEL_HK_PLAYER_UP, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_player_up_option},      // 4
    {{LABEL_HK_PLAYER_DOWN, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_player_down_option},  // 5
    {{LABEL_HK_EXIT_GAME, NULL}, &visibility_visible, TYPE_OPTION_CHECK_BOX, &hk_exit_game_option},      // 6
    {{LABEL_RESET_CONFIGS, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetHotkeyConfigCallback}, // 7
};
static SettingMenu hotkey_menu = {{TAB_HOTKEY, NULL}, &visibility_visible, hotkey_menu_items, sizeof(hotkey_menu_items) / sizeof(SettingMenuItem), NULL, hotkeyMenuExitCallback};

// 核心
static SettingMenu core_menu = {{TAB_CORE, NULL}, &core_menu_visibility, NULL, 0, NULL, coreMenuExitCallback};

// 杂项
static SettingMenuItem misc_menu_items[] = {
    {{LABEL_AUTO_SAVE_LOAD_STATE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &auto_save_load_option}, // 0
    {{LABEL_SAVE_SCREENSHOT, NULL}, &game_is_loaded, TYPE_OPTION_CALLBACK, saveScreenshotCallback},           // 1
    {{LABEL_SAVE_PREVIEW, NULL}, &game_is_loaded, TYPE_OPTION_CALLBACK, saveScreenshotForPreviewCallback},    // 2
    {{LABEL_RESET_CONFIGS, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetMiscConfigCallback},        // 3
};
static SettingMenu misc_menu = {{TAB_MISC, NULL}, &visibility_visible, misc_menu_items, sizeof(misc_menu_items) / sizeof(SettingMenuItem), NULL, miscMenuExitCallback};

// 程序
static SettingMenuItem app_menu_items[] = {
    {{LABEL_PREVIEW_PATH, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &preview_path_option},
    {{LABEL_PREVIEW_STYLE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &preview_style_option},
    {{LABEL_APP_LOG, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &app_log_option},
    {{LABEL_CORE_LOG, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &core_log_option},
    {{LABEL_SHOW_LOG, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &show_log_option},
    {{LABEL_LANGUAGE, NULL}, &visibility_visible, TYPE_OPTION_STR_ARRAY, &language_option},
    {{LABEL_RESET_CONFIGS, NULL}, &visibility_visible, TYPE_OPTION_CALLBACK, resetAppConfigCallback},
};
static SettingMenu app_menu = {{TAB_APP, NULL}, &app_menu_visibility, app_menu_items, sizeof(app_menu_items) / sizeof(SettingMenuItem), NULL, appMenuExitCallback};

// 菜单列表
static SettingMenu *setting_menus[] = {
    &main_menu,     // 主菜单
    &state_menu,    // 即时存档
    &graphics_menu, // 图形
    &control_menu,  // 控制
    &hotkey_menu,   // 热键
    &core_menu,     // 核心
    &misc_menu,     // 杂项
    &app_menu,      // 程序
};
#define MENU_LIST_LEN (sizeof(setting_menus) / sizeof(SettingMenu *))
