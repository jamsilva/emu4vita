#ifndef __M_ABOUT_TEXT_H__
#define __M_ABOUT_TEXT_H__

#include "config.h"

char *about_texts[] = {
    APP_NAME_STR,
    " * 作者: 一直改 (百度psvita破解吧)",
    " * 版本: " APP_VER_STR "    日期: " BUILD_DATE,
    "",
    "平台:",
    " * " CORE_SOFTWARE,
    "",
    "操作:",
    " * 按“home键”->呼出菜单",
    " * 游戏中: 按“home键+□”->即时存档",
    " * 游戏中: 按“home键+△”->即时读档",
    " * 游戏中: 按“home键+X”->退出游戏",
    " * 游戏中: 按“home键+R”->加速游戏",
    " * 游戏中: 按“home键+L”->减速游戏",
    " * 游戏中: 按“home键+右摇杆左”->切换玩家-",
    " * 游戏中: 按“home键+右摇杆右”->切换玩家+",
    "",
    "感谢:",
    " * Team molecule (HENkaku)",
    " * xerpi (vita2dlib)",
    " * TheFlow (vitaShell)",
    " * libretro (core)",
    " * frangarcj (vita shader)",
    " * 永爱水原ありさ (图标及素材)",
    " * ......",
};

#define N_ABOUT_TEXTS (sizeof(about_texts) / sizeof(char **))

#endif