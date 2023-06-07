#include <string.h>

#include "lang.h"

char *lang_us[LANGUAGE_CONTAINER_SIZE] = {

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE_0
    "Currently in safe mode, please enable unsafe homemade software in HENkaku settings first,",
    // MESSAGE_SAFE_MODE_1
    "only then can this program be used normally.",
    // MESSAGE_SAFE_MODE_2
    "Press any key to exit!",

    /**************  Button string  *****************/
    // BUTTON_LEFT
    "←",
    // BUTTON_UP
    "↑",
    // BUTTON_RIGHT
    "→",
    // BUTTON_DOWN
    "↓",
    // BUTTON_CROSS
    "X", //"×"
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
    // BUTTON_AND
    "+",
    // BUTTON_OR
    "/",

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

    // SWICTH_MODE
    "Swicth mode",
    // COIN
    "Coin",
    // SELECT
    "Select",
    // START
    "Start",
    // TURBO
    "Turbo",

    /**************  Browser  *****************/
    // PARENT_DIRECTORY
    "Parent dir",
    // OPEN
    "Open",
    // EMU_SETTING
    "Emu setting",
    // ABOUT
    "About",
    // CHANGE_DIRECTORY
    "Change dir",
    // BACK_TO_BROWSER
    "Back",

    /**************  Setting tab  *****************/
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
    // YES
    "Yes",
    // NO
    "No",
    // DISABLE
    "Disable",
    // DEFAULT
    "Default",
    // AUTO
    "Auto",
    // NONE
    "None",
    // RESET_CONFIGS
    "Reset configs",

    /**************  Menu main  *****************/
    // RESUME_GAME
    "Resume game",
    // RESET_GAME
    "Reset game",
    // EXIT_GAME
    "Exit game",
    // EXIT_TO_ARCH
    "Eixt to arch",
    // EXIT_APP
    "Exit app",

    /**************  Menu grahics  *****************/
    // DISPLAY_SIZE
    "Display size",
    // DISPLAY_RATIO
    "Display ratio",
    // DISPLAY_ROTATE
    "Display rotate",
    // GRAHICS_FILTERING
    "Grahics filtering",
    // GRAHICS_SMOOTH
    "Grahics smooth",
    // OVERLAY_SELECT
    "Overlay select",
    // OVERLAY_MODE
    "Overlay mode",
    // SHOW_FPS
    "Show FPS",

    // DISPLAY_SIZE_1X
    "1X",
    // DISPLAY_SIZE_2X
    "2X",
    // DISPLAY_SIZE_3X
    "3X",
    // DISPLAY_SIZE_FULL
    "Full",

    // DISPLAY_RATIO_DEFAULT
    "Default",
    // DISPLAY_RATIO_BY_GAME_RESOLUTION
    "By game resolution",
    // DISPLAY_RATIO_BY_DEV_SCREEN
    "By device screen",
    // DISPLAY_RATIO_8_7
    "8:7",
    // DISPLAY_RATIO_4_3
    "4:3",
    // DISPLAY_RATIO_3_2
    "3:2",
    // DISPLAY_RATIO_16_9
    "16:9",

    // ROTATE_90
    "Rotate 90",
    // ROTATE_180
    "Rotate 180",
    // ROTATE_270
    "Rotate 270",

    // SHADER_LCD3X
    "LCD3x",
    // SHADER_SHARP_SIMPLE
    "Sharp bilinear simple",
    // SHADER_SHARP
    "Sharp bilinear",
    // SHADER_AA
    "Advanced AA",

    // OVERLAY_MODE_OVERLAY
    "Overlay",
    // OVERLAY_MODE_BACKGROUND
    "Background",

    /**************  Menu control  *****************/
    // CTRL_PLAYER
    "Control player",
    // FRONT_TOUCH_TO_PAD
    "Front touch to pad",
    // BACK_TOUCH_TO_PAD
    "Back touch to pad",
    // TURBO_DELAY
    "Turbo delay (frame)",
    // RESET_CTRL_CONFIGS_H
    "Reset configs (H)",
    // RESET_CTRL_CONFIGS_V
    "Reset configs (V)",

    // FOLLOW_DPAD
    "Follow DPAD",

    /**************  Menu misc  *****************/
    // AUTO_STATE
    "Auto state",
    // DELETE_AUTO_STATE
    "Delete auto state",
    // SAVE_SCREENSHOT
    "Save screenshot",
    // SAVE_PREVIEW
    "Save screenshot for preview",

    /**************  Menu app  *****************/
    // PREVIEW_PATH
    "Preview path",
    // PREVIEW_STYLE
    "Preview style",
    // APP_LOG
    "App log",
    // CORE_LOG
    "Core log",
    // PRINT_LOG
    "Print log (loading)",
    // LANGUAGE
    "language",

    // FROM_AUTO_STATE
    "From auto state",

    // PRESERVE_FULL
    "Preserve full",
    // STRETCH_FULL
    "Stretch full",

    /**************  Menu state  *****************/
    // SAVESTATE_EXIST
    "State",
    // SAVESTATE_NOEXIST
    "None",

    // LOAD
    "Load",
    // SAVE
    "Save",
    // DELETE
    "Delete",
    // CANCEL
    "Cancel",

};

char *lang_chs[LANGUAGE_CONTAINER_SIZE] = {

    /**************  Safe mode  *****************/
    // MESSAGE_SAFE_MODE_0
    "当前处于安全模式，请先在HENkaku设置里开启启用不安全自制软件，",
    // MESSAGE_SAFE_MODE_1
    "然后才能正常使用本程序。",
    // MESSAGE_SAFE_MODE_2
    "按任意键退出！",

    /**************  Button string  *****************/
    // BUTTON_LEFT
    "←",
    // BUTTON_UP
    "↑",
    // BUTTON_RIGHT
    "→",
    // BUTTON_DOWN
    "↓",
    // BUTTON_CROSS
    "X", //"×"
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
    // BUTTON_AND
    "+",
    // BUTTON_OR
    "/",

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

    // SWICTH_MODE
    "模式切换",
    // COIN
    "投币",
    // SELECT
    "选择",
    // START
    "开始",
    // TURBO
    "连发",

    /**************  Browser  *****************/
    // PARENT_DIRECTORY
    "上层目录",
    // OPEN
    "打开",
    // EMU_SETTING
    "模拟器菜单",
    // ABOUT
    "关于",
    // CHANGE_DIRECTORY
    "跳转目录",
    // BACK_TO_BROWSER
    "返回浏览器",

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
    // YES
    "是",
    // NO
    "否",
    // DISABLE
    "禁用",
    // DEFAULT
    "默认",
    // AUTO
    "自动",
    // NONE
    "无",
    // RESET_CONFIGS
    "恢复默认设置",

    /**************  Menu main  *****************/
    // RESUME_GAME
    "继续游戏",
    // RESET_GAME
    "重置游戏",
    // EXIT_GAME
    "退出游戏",
    // EXIT_TO_ARCH
    "返回前端",
    // EXIT_APP
    "退出程序",

    /**************  Menu grahics  *****************/
    // DISPLAY_SIZE
    "画面尺寸",
    // DISPLAY_RATIO
    "画面比例",
    // DISPLAY_ROTATE
    "画面旋转",
    // GRAHICS_FILTERING
    "图像滤境",
    // GRAHICS_SMOOTH
    "平滑图像",
    // OVERLAY_SELECT
    "遮罩图选择",
    // OVERLAY_MODE
    "遮罩图显示模式",
    // SHOW_FPS
    "显示帧数",

    // DISPLAY_SIZE_1X
    "1倍大小",
    // DISPLAY_SIZE_2X
    "2倍大小",
    // DISPLAY_SIZE_3X
    "3倍大小",
    // DISPLAY_SIZE_FULL
    "铺满屏幕",

    // DISPLAY_RATIO_DEFAULT
    "默认",
    // DISPLAY_RATIO_BY_GAME_RESOLUTION
    "由游戏分辨率",
    // DISPLAY_RATIO_BY_DEV_SCREEN
    "由设备屏幕",
    // DISPLAY_RATIO_8_7
    "8:7",
    // DISPLAY_RATIO_4_3
    "4:3",
    // DISPLAY_RATIO_3_2
    "3:2",
    // DISPLAY_RATIO_16_9
    "16:9",

    // ROTATE_90
    "旋转90度",
    // ROTATE_180
    "旋转180度",
    // ROTATE_270
    "旋转270度",

    // SHADER_LCD3X
    "LCD3x",
    // SHADER_SHARP_SIMPLE
    "锐利双线性",
    // SHADER_SHARP
    "锐利双线性+扫描线",
    // SHADER_AA
    "高级AA",

    // OVERLAY_MODE_OVERLAY
    "覆层模式",
    // OVERLAY_MODE_BACKGROUND
    "背景模式",

    /**************  Menu control  *****************/
    // CTRL_PLAYER
    "玩家控制",
    // FRONT_TOUCH_TO_PAD
    "前触摸补键",
    // BACK_TOUCH_TO_PAD
    "背触摸补键",
    // TURBO_DELAY
    "连发间隔 (帧)",
    // RESET_CTRL_CONFIGS_H
    "恢复默认设置 (横屏)",
    // RESET_CTRL_CONFIGS_V
    "恢复默认设置 (竖屏)",

    // FOLLOW_DPAD
    "跟随十字键",

    /**************  Menu misc  *****************/
    // AUTO_STATE
    "自动存读档",
    // DELETE_AUTO_STATE
    "删除自动存档",
    // SAVE_SCREENSHOT
    "保存截图",
    // SAVE_PREVIEW
    "保存截图为预览图",

    /**************  Menu app  *****************/
    // PREVIEW_PATH
    "预览图路径",
    // PREVIEW_STYLE
    "预览图样式",
    // APP_LOG
    "程序日志",
    // CORE_LOG
    "核心日志",
    // PRINT_LOG
    "显示日志 (加载游戏时)",
    // LANGUAGE
    "语言",

    // FROM_AUTO_STATE
    "从自动存档",

    // PRESERVE_FULL
    "等比铺满",
    // STRETCH_FULL
    "拉伸铺满",

    /**************  Menu state  *****************/
    // SAVESTATE_EXIST
    "已存档",
    // SAVESTATE_NOEXIST
    "未存档",

    // LOAD
    "读取",
    // SAVE
    "保存",
    // DELETE
    "删除",
    // CANCEL
    "取消",

};

LangEntry lang_entries[] = {
    {"English", lang_us},
    {"Japanese", NULL},
    {"French", NULL},
    {"Spanish", NULL},
    {"German", NULL},
    {"Italian", NULL},
    {"Dutch", NULL},
    {"Portuguese(brazil)", NULL},
    {"Portuguese(portugal)", NULL},
    {"Russian", NULL},
    {"Korean", NULL},
    {"繁體中文", NULL},
    {"简体中文", lang_chs},
    {"Esperanto", NULL},
    {"Polish", NULL},
    {"Vietnamese", NULL},
    {"Arabic", NULL},
    {"Greek", NULL},
    {"Turkish", NULL},
    {"Slovak", NULL},
    {"Persian", NULL},
    {"Hebrew", NULL},
    {"Asturian", NULL},
    {"Finnish", NULL},
};

#define N_LANGS (sizeof(lang_entries) / sizeof(LangEntry))

char **cur_lang = lang_chs;

int GetLangsLength()
{
    return N_LANGS;
}

int GetLangValue(int id)
{
    int index = 0;
    int length = N_LANGS;

    int i;
    for (i = 0; i < length && index != id; i++)
    {
        if (lang_entries[i].container)
            index++;
    }
    if (id < 0 || id > index || i >= length)
        return 0;

    return i;
}

int SetCurrentLang(int id)
{
    int index = 0;
    int length = N_LANGS;

    int i;
    for (i = 0; i < length; i++)
    {
        if (lang_entries[i].container)
        {
            if (index == id)
                break;
            else
                index++;
        }
    }
    if (id < 0 || id > index || i >= length)
        return -1;

    cur_lang = lang_entries[i].container;
    return 0;
}