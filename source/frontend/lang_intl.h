#ifndef __M_LANG_INTL_H__
#define __M_LANG_INTL_H__

#include "lang.h"

char *lang_us[LANGUAGE_CONTAINER_SIZE] = {
    // Null
    NULL,

    /**************  General  *****************/
    // YES
    "Yes",
    // NO
    "No",
    // CONFIRM
    "Confirm",
    // CANCEL
    "Cancel",
    // BACK
    "Back",
    // EXIT
    "Exit",

    /**************  Alert dialog  *****************/
    // TITLE_TIP
    "Tip",
    // TITLE_MENU
    "Menu",
    // COLSE
    "Close",

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE
    "To run this app, go to the Settings app > HENkaku Settings and enable unsafe applications.",

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
    // LABEL_FDS_DISK_SIDE_CHANGE
    "(FDS) Disk side change",
    // LABEL_FDS_INSERT_EJECT_DISK
    "(FDS) Insert/Eject disk",

    /**************  Browser  *****************/
    // APP_TITLE
    APP_NAME_STR " v" APP_VER_STR,

    // PARENT_DIRECTORY
    "Go up a folder",
    // OPEN_DIR
    "Open current folder",
    // START_GAME
    "Run game",
    // OPTION_MENU
    "Options",
    // SETTING_MENU
    "Settings",
    // ABOUT
    "Info",
    // CHANGE_DIR
    "Go to last played game",

    // LABEL_START_GAME
    "Run game",
    // LABEL_DELETE_GAME
    "Delete game",
    // LABEL_DELETE_AUTO_STATE
    "Delete save state",
    // LABEL_DELETE_SAVEFILE
    "Delete savefile",

    // MESSAGE_ASK_DELETE_GAME
    "Do you really want to delete this game?",
    // MESSAGE_ASK_DELETE_AUTO_STATE
    "Do you really want to delete this game's save state?",
    // MESSAGE_ASK_DELETE_SAVEFILE
    "Do you really want to delete this game's save file?",
    // MESSAGE_START_GAME_FAILED
    "Failure when trying to run the game!",

    /**************  About  *****************/
    // ABOUT_TITLE
    "About",

    /**************  Setting table  *****************/
    // TAB_MAIN
    "Main",
    // TAB_STATE
    "Save states",
    // TAB_GRAPHICS
    "Video",
    // TAB_CONTROL
    "Input",
    // TAB_HOTKEY
    "Shortcuts",
    // TAB_CORE
    "Core options",
    // TAB_MISC
    "Misc",
    // TAB_APP
    "App",

    /**************  Menu general  *****************/
    // DISABLE
    "Disable",
    // DEFAULT
    "Default",
    // AUTO
    "Auto",
    // NONE
    "None",
    // LABEL_RESET_CONFIGS
    "Reset configs",

    /**************  Menu main  *****************/
    // LABEL_RESUME_GAME
    "Resume game",
    // LABEL_RESET_GAME
    "Reset game",
    // LABEL_EXIT_GAME
    "Close game",
    // LABEL_DISK_CONTROL
    "Disk control",
    // LABEL_EXIT_TO_ARCH
    "Exit to arch",
    // LABEL_EXIT_APP
    "Exit app",

    // TITLE_SWITCH_DISK
    "Swap disk",
    // LABEL_DISK
    "Disk",
    // CURRENT
    "Current",

    /**************  Menu graphics  *****************/
    // LABEL_DISPLAY_SIZE
    "Display scaling",
    // LABEL_ASPECT_RATIO
    "Aspect ratio",
    // LABEL_DISPLAY_ROTATE
    "Display rotate",
    // LABEL_GRAHICS_SHADER
    "Graphics shader",
    // LABEL_GRAHICS_SMOOTH
    "Graphics smoothing",
    // LABEL_OVERLAY_SELECT
    "Overlay image",
    // LABEL_OVERLAY_MODE
    "Overlay mode",
    // LABEL_SHOW_FPS
    "Show FPS",

    // DISPLAY_SIZE_1X
    "1x",
    // DISPLAY_SIZE_2X
    "2x",
    // DISPLAY_SIZE_3X
    "3x",
    // DISPLAY_SIZE_FULL
    "Full screen",

    // ASPECT_RATIO_BY_GAME_RESOLUTION
    "By game resolution",
    // ASPECT_RATIO_BY_DEV_SCREEN
    "By device screen",
    // ASPECT_RATIO_8_7
    "8:7",
    // ASPECT_RATIO_4_3
    "4:3",
    // ASPECT_RATIO_3_2
    "3:2",
    // ASPECT_RATIO_16_9
    "16:9",

    // DISPLAY_ROTATE_CW_90
    "Rotate 90° clockwise",
    // DISPLAY_ROTATE_CW_180
    "Rotate 180° clockwise",
    // DISPLAY_ROTATE_CW_270
    "Rotate 270° clockwise",

    // SHADER_LCD3X
    "LCD3x",
    // SHADER_SHARP_BILINEAR_SIMPLE
    "Simple sharp bilinear",
    // SHADER_SHARP_BILINEAR
    "Sharp bilinear",
    // SHADER_ADVANCED_AA
    "Advanced anti-aliasing",

    // OVERLAY_MODE_OVERLAY
    "Foreground",
    // OVERLAY_MODE_BACKGROUND
    "Background",

    /**************  Menu control  *****************/
    // LABEL_CTRL_PLAYER
    "Control player",
    // LABEL_FRONT_TOUCH_TO_BUTTON
    "Front touch to button",
    // LABEL_BACK_TOUCH_TO_BUTTON
    "Back touch to button",
    // LABEL_TURBO_DELAY
    "Turbo delay (frames)",

    // FOLLOW_DPAD
    "Follow D-Pad",

    /**************  Menu misc  *****************/
    // LABEL_AUTO_SAVE_LOAD_STATE
    "Auto save/load state",
    // LABEL_SAVE_SCREENSHOT
    "Take screenshot",
    // LABEL_SAVE_PREVIEW
    "Preview screenshot",
    // LABEL_HK_SAVESTATE
    "Save state shortcut",
    // LABEL_HK_LOADSTATE
    "Load state shortcut",
    // LABEL_HK_SPEED_UP
    "Speed up shortcut",
    // LABEL_HK_SPEED_DOWN
    "Speed down shortcut",
    // LABEL_HK_PLAYER_UP
    "Control player+ shortcut",
    // LABEL_HK_PLAYER_DOWN
    "Control player- shortcut",
    // LABEL_HK_EXIT_GAME
    "Close game shortcut",

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
    "From save state",

    // FULL_PRESERVE
    "Fit screen",
    // FULL_STRETCH
    "Stretch",
    // FULL_CROP,
    "Fill screen and crop",

    /**************  Menu state  *****************/
    // LABEL_EXISTENT_STATE
    "Save state",
    // LABEL_NON_EXISTENT_STATE
    "Empty",

    // LOAD
    "Load",
    // SAVE
    "Save",
    // DELETE
    "Delete",
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
    // EXIT
    "退出",

    /**************  Dialog  *****************/
    // TITLE_TIP
    "提示",
    // TITLE_MENU
    "菜单",
    // COLSE
    "关闭",

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE
    "当前处于安全模式，请先在HENkaku设置里开启启用不安全自制软件，"
    "然后才能正常使用本程序。",

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
    // LABEL_FDS_DISK_SIDE_CHANGE
    "(FDS) 更换磁盘面",
    // LABEL_FDS_INSERT_EJECT_DISK
    "(FDS) 插入/弹出磁盘",

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
    // TAB_HOTKEY
    "热键",
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

    // TITLE_SWITCH_DISK
    "更换光盘",
    // LABEL_DISK
    "光盘",
    // CURRENT
    "当前",

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
    // FULL_CROP,
    "裁剪铺满",

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
