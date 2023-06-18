#ifndef __M_LANG_INTL_H__
#define __M_LANG_INTL_H__

#include "lang.h"

char *lang_us[LANGUAGE_CONTAINER_SIZE] = {
    // Null
    NULL,

    /**************  General  *****************/
    // YES
    "yes",
    // NO
    "no",
    // CONFIRM
    "confirm",
    // CANCEL
    "cancel",
    // BACK
    "back",

    /**************  Alert dialog  *****************/
    // TITLE_TIP
    "Tip",
    // TITLE_MENU
    "Menu",
    // COLSE
    "close",

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE_0
    "Currently in safe mode, please enable unsafe homemade software in HENkaku settings first,",
    // MESSAGE_SAFE_MODE_1
    "only then can this program be used normally.",
    // MESSAGE_SAFE_MODE_2
    "Press any key to exit!",

    /**************  Button string  *****************/
    // BUTTON_ENTER
    NULL,
    // BUTTON_CANCEL
    NULL,
    // BUTTON_LEFT
    "←",
    // BUTTON_UP
    "↑",
    // BUTTON_RIGHT
    "→",
    // BUTTON_DOWN
    "↓",
    // BUTTON_CROSS
    "×",
    // BUTTON_CIRCLE
    "○",
    // BUTTON_SQUARE
    "□",
    // BUTTON_TRIANGLE
    "△",
    // BUTTON_L
    "L",
    // BUTTON_R
    "R",
    // BUTTON_L2
    "L2",
    // BUTTON_R2
    "R2",
    // BUTTON_L3
    "L3",
    // BUTTON_R3
    "R3",
    // BUTTON_SELECT
    "Select",
    // BUTTON_START
    "Start",
    // BUTTON_PSBUTTON
    "Home",
    // BUTTON_LEFT_ANALOG
    "Left analog",
    // BUTTON_LEFT_ANALOG_LEFT
    "Left analog←",
    // BUTTON_LEFT_ANALOG_UP
    "Left analog↑",
    // BUTTON_LEFT_ANALOG_RIGHT
    "Left analog→",
    // BUTTON_LEFT_ANALOG_DOWN
    "Left analog↓",
    // BUTTON_RIGHT_ANALOG
    "Right analog",
    // BUTTON_RIGHT_ANALOG_LEFT
    "Right analog←",
    // BUTTON_RIGHT_ANALOG_UP
    "Right analog↑",
    // BUTTON_RIGHT_ANALOG_RIGHT
    "Right analog→",
    // BUTTON_RIGHT_ANALOG_DOWN
    "Right analog↓",

    /**************  Button string 2  *****************/
    // BUTTON_A
    "A",
    // BUTTON_B
    "B",
    // BUTTON_C
    "C",
    // BUTTON_D
    "D",
    // BUTTON_E
    "E",
    // BUTTON_F
    "F",
    // BUTTON_G
    "G",
    // BUTTON_H
    "H",
    // BUTTON_X
    "X",
    // BUTTON_Y
    "Y",
    // BUTTON_Z
    "Z",

    // BUTTON_X1
    "X1",
    // BUTTON_X2
    "X2",
    // BUTTON_X3
    "X3",
    // BUTTON_X4
    "X4",
    // BUTTON_Y1
    "Y1",
    // BUTTON_Y2
    "Y2",
    // BUTTON_Y3
    "Y3",
    // BUTTON_Y4
    "Y4",

    // LABEL_SWICTH_MODE
    "Swicth mode",
    // LABEL_COIN
    "Coin",
    // LABEL_SELECT
    "Select",
    // LABEL_START
    "Start",
    // LABEL_TURBO
    "Turbo",

    /**************  Browser  *****************/
    // APP_TITLE
    APP_NAME_STR " v" APP_VER_STR,

    // PARENT_DIRECTORY
    "parent dir",
    // OPEN_DIR
    "open dir",
    // START_GAME
    "start game",
    // OPTION_MENU
    "option menu",
    // SETTING_MENU
    "setting menu",
    // ABOUT
    "about",
    // CHANGE_DIR
    "change dir",

    // LABEL_START_GAME
    "Start game",
    // LABEL_DELETE_GAME
    "Delete game",
    // LABEL_DELETE_AUTO_STATE
    "Delete auto state",
    // LABEL_DELETE_SAVEFILE
    "Delete savefile",

    // MESSAGE_START_GAME_FAILED
    "Failed to start game!",
    // MESSAGE_ASK_DELETE_GAME
    "Are you sure to delete this game?",
    // MESSAGE_ASK_DELETE_AUTO_STATE
    "Are you sure to delete the auto state of this game?",
    // MESSAGE_ASK_DELETE_SAVEFILE
    "Are you sure to delete the save file of this game?",

    /**************  About  *****************/
    // ABOUT_TITLE
    "About",

    /**************  Setting table  *****************/
    // TAB_MAIN
    "Main",
    // TAB_STATE
    "State",
    // TAB_GRAPHICS
    "Graphics",
    // TAB_CONTROL
    "Control",
    // TAB_CORE
    "Core",
    // TAB_MISC
    "Misc",
    // TAB_APP
    "App",

    /**************  Menu general  *****************/
    // DISABLE
    "disable",
    // DEFAULT
    "default",
    // AUTO
    "auto",
    // NONE
    "none",
    // LABEL_RESET_CONFIGS
    "Reset configs",

    /**************  Menu main  *****************/
    // LABEL_RESUME_GAME
    "Resume game",
    // LABEL_RESET_GAME
    "Reset game",
    // LABEL_EXIT_GAME
    "Exit game",
    // LABEL_DISK_CONTROL
    "Disk control",
    // LABEL_EXIT_TO_ARCH
    "Eixt to arch",
    // LABEL_EXIT_APP
    "Exit app",

    /**************  Menu grahics  *****************/
    // LABEL_DISPLAY_SIZE
    "Display size",
    // LABEL_ASPECT_RATIO
    "Aspect ratio",
    // LABEL_DISPLAY_ROTATE
    "Display rotate",
    // LABEL_GRAHICS_SHADER
    "Grahics shader",
    // LABEL_GRAHICS_SMOOTH
    "Grahics smooth",
    // LABEL_OVERLAY_SELECT
    "Overlay select",
    // LABEL_OVERLAY_MODE
    "Overlay mode",
    // LABEL_SHOW_FPS
    "Show FPS",

    // DISPLAY_SIZE_1X
    "1X",
    // DISPLAY_SIZE_2X
    "2X",
    // DISPLAY_SIZE_3X
    "3X",
    // DISPLAY_SIZE_FULL
    "full",

    // ASPECT_RATIO_BY_GAME_RESOLUTION
    "by game resolution",
    // ASPECT_RATIO_BY_DEV_SCREEN
    "by device screen",
    // ASPECT_RATIO_8_7
    "8:7",
    // ASPECT_RATIO_4_3
    "4:3",
    // ASPECT_RATIO_3_2
    "3:2",
    // ASPECT_RATIO_16_9
    "16:9",

    // DISPLAY_ROTATE_CW_90
    "rotate 90°CW",
    // DISPLAY_ROTATE_CW_180
    "rotate 180°CW",
    // DISPLAY_ROTATE_CW_270
    "rotate 270°CW",

    // SHADER_LCD3X
    "LCD3x",
    // SHADER_SHARP_BILINEAR_SIMPLE
    "sharp bilinear simple",
    // SHADER_SHARP_BILINEAR
    "sharp bilinear",
    // SHADER_ADVANCED_AA
    "advanced AA",

    // OVERLAY_MODE_OVERLAY
    "overlay",
    // OVERLAY_MODE_BACKGROUND
    "background",

    /**************  Menu control  *****************/
    // LABEL_CTRL_PLAYER
    "Control player",
    // LABEL_FRONT_TOUCH_TO_BUTTON
    "Front touch to button",
    // LABEL_BACK_TOUCH_TO_BUTTON
    "Back touch to button",
    // LABEL_TURBO_DELAY
    "Turbo delay (frame)",
    // LABEL_RESET_CONFIGS_H
    "Reset configs (H)",
    // LABEL_RESET_CONFIGS_V
    "Reset configs (V)",

    // FOLLOW_DPAD
    "follow d-pad",

    /**************  Menu misc  *****************/
    // LABEL_AUTO_SAVE_LOAD_STATE
    "Auto save/load state",
    // LABEL_SAVE_SCREENSHOT
    "Save screenshot",
    // LABEL_SAVE_PREVIEW
    "Save screenshot for preview",
    // LABEL_HK_SAVESTATE
    "Save state (hot key)",
    // LABEL_HK_LOADSTATE
    "Load state (hot key)",
    // LABEL_HK_SPEED_UP
    "Speed up (hot key)",
    // LABEL_HK_SPEED_DOWN
    "Speed down (hot key)",
    // LABEL_HK_PLAYER_UP
    "Control player+ (hot key)",
    // LABEL_HK_PLAYER_DOWN
    "Control player- (hot key)",
    // LABEL_HK_EXIT_GAME
    "Exit game (hot key)",

    /**************  Menu app  *****************/
    // LABEL_PREVIEW_PATH
    "Preview path",
    // LABEL_PREVIEW_STYLE
    "Preview style",
    // LABEL_APP_LOG
    "App log",
    // LABEL_CORE_LOG
    "Core log",
    // LABEL_SHOW_LOG
    "Show log (loading)",
    // LABEL_LANGUAGE
    "Language",

    // FROM_AUTO_STATE
    "from auto state",

    // FULL_PRESERVE
    "preserve full",
    // FULL_STRETCH
    "stretch full",
    // FULL_4_3,
    "4:3 full",
    // FULL_3_2
    "3:2 full",

    /**************  Menu state  *****************/
    // LABEL_EXISTENT_STATE
    "State",
    // LABEL_NON_EXISTENT_STATE
    "None",

    // LOAD
    "load",
    // SAVE
    "save",
    // DELETE
    "delete",
};

char *lang_chs[LANGUAGE_CONTAINER_SIZE] = {
    // Null
    NULL,

    /**************  General  *****************/
    // YES
    "是",
    // NO
    "否",
    // CONFIRM
    "确定",
    // CANCEL
    "取消",
    // BACK
    "返回",

    /**************  Dialog  *****************/
    // TITLE_TIP
    "提示",
    // TITLE_MENU
    "菜单",
    // COLSE
    "关闭",

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE_0
    "当前处于安全模式，请先在HENkaku设置里开启启用不安全自制软件，",
    // MESSAGE_SAFE_MODE_1
    "然后才能正常使用本程序。",
    // MESSAGE_SAFE_MODE_2
    "按任意键退出！",

    /**************  Button string  *****************/
    // BUTTON_ENTER
    NULL,
    // BUTTON_CANCEL
    NULL,
    // BUTTON_LEFT
    "←",
    // BUTTON_UP
    "↑",
    // BUTTON_RIGHT
    "→",
    // BUTTON_DOWN
    "↓",
    // BUTTON_CROSS
    "×",
    // BUTTON_CIRCLE
    "○",
    // BUTTON_SQUARE
    "□",
    // BUTTON_TRIANGLE
    "△",
    // BUTTON_L
    "L",
    // BUTTON_R
    "R",
    // BUTTON_L2
    "L2",
    // BUTTON_R2
    "R2",
    // BUTTON_L3
    "L3",
    // BUTTON_R3
    "R3",
    // BUTTON_SELECT
    "Select",
    // BUTTON_START
    "Start",
    // BUTTON_PSBUTTON
    "Home",
    // BUTTON_LEFT_ANALOG
    "左摇杆",
    // BUTTON_LEFT_ANALOG_LEFT
    "左摇杆←",
    // BUTTON_LEFT_ANALOG_UP
    "左摇杆↑",
    // BUTTON_LEFT_ANALOG_RIGHT
    "左摇杆→",
    // BUTTON_LEFT_ANALOG_DOWN
    "左摇杆↓",
    // BUTTON_RIGHT_ANALOG
    "右摇杆",
    // BUTTON_RIGHT_ANALOG_LEFT
    "右摇杆←",
    // BUTTON_RIGHT_ANALOG_UP
    "右摇杆↑",
    // BUTTON_RIGHT_ANALOG_RIGHT
    "右摇杆→",
    // BUTTON_RIGHT_ANALOG_DOWN
    "右摇杆↓",

    /**************  Button string 2  *****************/
    // BUTTON_A
    "A",
    // BUTTON_B
    "B",
    // BUTTON_C
    "C",
    // BUTTON_D
    "D",
    // BUTTON_E
    "E",
    // BUTTON_F
    "F",
    // BUTTON_G
    "G",
    // BUTTON_H
    "H",
    // BUTTON_X
    "X",
    // BUTTON_Y
    "Y",
    // BUTTON_Z
    "Z",

    // BUTTON_X1
    "X1",
    // BUTTON_X2
    "X2",
    // BUTTON_X3
    "X3",
    // BUTTON_X4
    "X4",
    // BUTTON_Y1
    "Y1",
    // BUTTON_Y2
    "Y2",
    // BUTTON_Y3
    "Y3",
    // BUTTON_Y4
    "Y4",

    // LABEL_SWICTH_MODE
    "模式切换",
    // LABEL_COIN
    "投币",
    // LABEL_SELECT
    "选择",
    // LABEL_START
    "开始",
    // LABEL_TURBO
    "连发",

    /**************  Browser  *****************/
    // APP_TITLE
    APP_NAME_STR " v" APP_VER_STR,

    // PARENT_DIRECTORY
    "上层目录",
    // OPEN_DIR
    "打开目录",
    // START_GAME
    "启动游戏",
    // OPTION_MENU
    "选项菜单",
    // SETTING_MENU
    "设置菜单",
    // ABOUT
    "关于",
    // CHANGE_DIR
    "跳转目录",

    // LABEL_START_GAME
    "启动游戏",
    // LABEL_DELETE_GAME
    "删除游戏",
    // LABEL_DELETE_AUTO_STATE
    "删除自动存档",
    // LABEL_DELETE_SAVEFILE
    "删除模拟存档",

    // MESSAGE_ASK_DELETE_GAME
    "确认要删除这个游戏？",
    // MESSAGE_ASK_DELETE_AUTO_STATE
    "确认要删除这个游戏的自动存档？",
    // MESSAGE_ASK_DELETE_SAVEFILE
    "确认要删除这个游戏的模拟存档？",
    // MESSAGE_START_GAME_FAILED
    "启动游戏失败！",

    /**************  About  *****************/
    // ABOUT_TITLE
    "关于",

    /**************  Setting tab  *****************/
    // TAB_MAIN
    "主菜单",
    // TAB_STATE
    "即时存档",
    // TAB_GRAPHICS
    "图形",
    // TAB_CONTROL
    "控制",
    // TAB_CORE
    "核心",
    // TAB_MISC
    "杂项",
    // TAB_APP
    "程序",

    /**************  Menu general  *****************/
    // DISABLE
    "禁用",
    // DEFAULT
    "默认",
    // AUTO
    "自动",
    // NONE
    "无",
    // LABEL_RESET_CONFIGS
    "恢复默认设置",

    /**************  Menu main  *****************/
    // LABEL_RESUME_GAME
    "继续游戏",
    // LABEL_RESET_GAME
    "重置游戏",
    // LABEL_EXIT_GAME
    "退出游戏",
    // LABEL_DISK_CONTROL
    "光盘控制",
    // LABEL_EXIT_TO_ARCH
    "返回前端",
    // LABEL_EXIT_APP
    "退出程序",

    /**************  Menu grahics  *****************/
    // LABEL_DISPLAY_SIZE
    "画面尺寸",
    // LABEL_ASPECT_RATIO
    "画面比例",
    // LABEL_DISPLAY_ROTATE
    "画面旋转",
    // LABEL_GRAHICS_SHADER
    "图像滤境",
    // LABEL_GRAHICS_SMOOTH
    "平滑图像",
    // LABEL_OVERLAY_SELECT
    "遮罩图选择",
    // LABEL_OVERLAY_MODE
    "遮罩图显示模式",
    // LABEL_SHOW_FPS
    "显示帧数",

    // DISPLAY_SIZE_1X
    "1倍大小",
    // DISPLAY_SIZE_2X
    "2倍大小",
    // DISPLAY_SIZE_3X
    "3倍大小",
    // DISPLAY_SIZE_FULL
    "铺满屏幕",

    // ASPECT_RATIO_BY_GAME_RESOLUTION
    "由游戏分辨率",
    // ASPECT_RATIO_BY_DEV_SCREEN
    "由设备屏幕",
    // ASPECT_RATIO_8_7
    "8:7",
    // ASPECT_RATIO_4_3
    "4:3",
    // ASPECT_RATIO_3_2
    "3:2",
    // ASPECT_RATIO_16_9
    "16:9",

    // DISPLAY_ROTATE_CW_90
    "旋转90度(顺时针)",
    // DISPLAY_ROTATE_CW_180
    "旋转180度(顺时针)",
    // DISPLAY_ROTATE_CW_270
    "旋转270度(顺时针)",

    // SHADER_LCD3X
    "LCD3x",
    // SHADER_SHARP_BILINEAR_SIMPLE
    "锐利双线性",
    // SHADER_SHARP_BILINEAR
    "锐利双线性+扫描线",
    // SHADER_ADVANCED_AA
    "高级AA",

    // OVERLAY_MODE_OVERLAY
    "覆层模式",
    // OVERLAY_MODE_BACKGROUND
    "背景模式",

    /**************  Menu control  *****************/
    // LABEL_CTRL_PLAYER
    "玩家控制",
    // LABEL_FRONT_TOUCH_TO_BUTTON
    "前触摸映射按键",
    // LABEL_BACK_TOUCH_TO_BUTTON
    "背触摸映射按键",
    // LABEL_TURBO_DELAY
    "连发间隔 (帧)",
    // LABEL_RESET_CONFIGS_H
    "恢复默认设置 (横屏)",
    // LABEL_RESET_CONFIGS_V
    "恢复默认设置 (竖屏)",

    // FOLLOW_DPAD
    "跟随十字键",

    /**************  Menu misc  *****************/
    // LABEL_AUTO_SAVE_LOAD_STATE
    "自动存读档",
    // LABEL_SAVE_SCREENSHOT
    "保存截图",
    // LABEL_SAVE_PREVIEW
    "保存截图为预览图",
    // LABEL_HK_SAVESTATE
    "保存存档 (快捷键)",
    // LABEL_HK_LOADSTATE
    "读取存档 (快捷键)",
    // LABEL_HK_SPEED_UP
    "加速游戏 (快捷键)",
    // LABEL_HK_SPEED_DOWN
    "减速游戏 (快捷键)",
    // LABEL_HK_PLAYER_UP
    "切换玩家+ (快捷键)",
    // LABEL_HK_PLAYER_DOWN
    "切换玩家- (快捷键)",
    // LABEL_HK_EXIT_GAME
    "退出游戏 (快捷键)",

    /**************  Menu app  *****************/
    // LABEL_PREVIEW_PATH
    "预览图路径",
    // LABEL_PREVIEW_STYLE
    "预览图样式",
    // LABEL_APP_LOG
    "程序日志",
    // LABEL_CORE_LOG
    "核心日志",
    // LABEL_SHOW_LOG
    "显示日志 (加载游戏时)",
    // LABEL_LANGUAGE
    "语言",

    // FROM_AUTO_STATE
    "从自动存档",

    // FULL_PRESERVE
    "等比铺满",
    // FULL_STRETCH
    "拉伸铺满",
    // FULL_4_3,
    "4:3铺满",
    // FULL_3_2
    "3:2铺满",

    /**************  Menu state  *****************/
    // LABEL_EXISTENT_STATE
    "已存档",
    // LABEL_NON_EXISTENT_STATE
    "未存档",

    // LOAD
    "读取",
    // SAVE
    "保存",
    // DELETE
    "删除",
};

#endif