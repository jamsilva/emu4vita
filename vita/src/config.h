#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__

#include <stdint.h>

#include <psp2/ctrl.h>

#include "List/config_list.h"
#include "List/option_list.h"
#include "List/overlay_list.h"
#include "config_text.h"
#include "file.h"

#define APP_CONFIG_VERSION 1
#define GRAPHICS_CONFIG_VERSION 2
#define CONTROL_CONFIG_VERSION 1
#define CORE_CONFIG_VERSION 1
#define MISC_CONFIG_VERSION 1

#define APP_CONFIG_NAME "app.cfg"
#define GRAPHICS_CONFIG_NAME "graphics.cfg"
#define CONTROL_CONFIG_NAME "control.cfg"
#define CORE_CONFIG_NAME "core.cfg"
#define MISC_CONFIG_NAME "misc.cfg"
#define OVERLAYS_CONFIG_NAME "overlays.cfg"

#define OVERLAYS_DIR_NAME "overlays"
#define PREVIEW_DIR_NAME "images"

#define CORE_ASSETS_DIR_NAME "assets"
#define CORE_SYSTEM_DIR_NAME "system"

#define FONT_PGF_NAME "font.pgf"
#define WALLPAPER_PNG_NAME "bg.png"
#define SPLASH_PNG_NAME "splash.png"

#define APP_DATA_DIR "ux0:data/EMU4VITA/" APP_DIR_NAME
#define APP_ASSETS_DIR "app0:assets"
#define CORE_DATA_DIR "app0:data"

#define CORE_CONFIGS_DIR APP_DATA_DIR "/configs"
#define CORE_SAVEFILES_DIR APP_DATA_DIR "/savefiles"
#define CORE_SAVESTATES_DIR APP_DATA_DIR "/savestates"
#define CORE_SCREENSHOTS_DIR APP_DATA_DIR "/screenshots"

#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"
#define CORE_LOG_PATH APP_DATA_DIR "/core_log.txt"
#define LASTFILE_PATH APP_DATA_DIR "/lastfile.txt"

#define MAX_CONFIG_KEY_LENGTH 256
#define MAX_CONFIG_VALUE_LENGTH 1024
#define MAX_CONFIG_LINE_LENGTH 2048

#define ENABLE_KEY_BITMASK (1 << 31)
#define TURBO_KEY_BITMASK (1 << 30)
#define RETRO_KEY_TO_BITMASK(id) (1 << id)

#define GET_BITMASK_KEY(id) ((1 << id) | ENABLE_KEY_BITMASK)
#define GET_BITMASK_KEY_TURBO(id) ((1 << id) | ENABLE_KEY_BITMASK | TURBO_KEY_BITMASK)

#define EXIT_APP_HOT_KEY (SCE_CTRL_PSBUTTON | SCE_CTRL_SELECT)
#define CHECK_EXIT_APP(key) ((key & EXIT_APP_HOT_KEY) == EXIT_APP_HOT_KEY)

#define DISABLE_PSBUTTON_EVENT_HOLD_COUNT 20

enum ExtCtrlButtons
{
    /* SCE_CTRL_PSBUTTON = SCE_CTRL_INTERCEPTED is 17, we have 30 bitmask for set */
    EXT_CTRL_LEFT_ANLOG_UP = SCE_CTRL_INTERCEPTED << 1,     // 18
    EXT_CTRL_LEFT_ANLOG_RIGHT = SCE_CTRL_INTERCEPTED << 2,  // 19
    EXT_CTRL_LEFT_ANLOG_DOWN = SCE_CTRL_INTERCEPTED << 3,   // 20
    EXT_CTRL_LEFT_ANLOG_LEFT = SCE_CTRL_INTERCEPTED << 4,   // 21
    EXT_CTRL_RIGHT_ANLOG_UP = SCE_CTRL_INTERCEPTED << 5,    // 22
    EXT_CTRL_RIGHT_ANLOG_RIGHT = SCE_CTRL_INTERCEPTED << 6, // 23
    EXT_CTRL_RIGHT_ANLOG_DOWN = SCE_CTRL_INTERCEPTED << 7,  // 24
    EXT_CTRL_RIGHT_ANLOG_LEFT = SCE_CTRL_INTERCEPTED << 8,  // 25
};

enum TypeDisplaySize
{
    TYPE_DISPLAY_SIZE_1X,
    TYPE_DISPLAY_SIZE_2X,
    TYPE_DISPLAY_SIZE_3X,
    TYPE_DISPLAY_SIZE_FULL,
};

enum TypeDisplayRatio
{
    TYPE_DISPLAY_RATIO_DEFAULT,
    TYPE_DISPLAY_RATIO_BY_GAME_RESOLUTION,
    TYPE_DISPLAY_RATIO_BY_DEV_SCREEN,
    TYPE_DISPLAY_RATIO_8_7,
    TYPE_DISPLAY_RATIO_4_3,
    TYPE_DISPLAY_RATIO_3_2,
    TYPE_DISPLAY_RATIO_16_9,
};

enum TypePreviewPath
{
    TYPE_PREVIEW_PATH_AUTO,
    TYPE_PREVIEW_PATH_DEFAULT,
    TYPE_PREVIEW_PATH_SAVESTATE,
};

enum TypePreviewStyle
{
    TYPE_PREVIEW_STYLE_PRESERVE_FULL,
    TYPE_PREVIEW_STYLE_STRETCH_FULL,
};

enum TypeConfig
{
    TYPE_CONFIG_MAIN,
    TYPE_CONFIG_GAME,
};

typedef struct
{
    uint32_t version;            // 0x00
    uint32_t display_size;       // 0x04
    uint32_t aspect_ratio;       // 0x08
    uint32_t display_rotate;     // 0x0C
    uint32_t graphics_filtering; // 0x10
    uint32_t graphics_smooth;    // 0x14
    uint32_t overlay_select;     // 0x18
    uint32_t overlay_mode;       // 0x1C
    uint32_t show_fps;           // 0x20
    char reserved[0x44];         // 0x24
} GraphicsConfig;                // 0x68

typedef struct
{
    uint32_t version;         // 0x00
    uint32_t button_up;       // 0x04
    uint32_t button_down;     // 0x08
    uint32_t button_left;     // 0x0C
    uint32_t button_right;    // 0x10
    uint32_t button_triangle; // 0x14
    uint32_t button_circle;   // 0x18
    uint32_t button_cross;    // 0x1C
    uint32_t button_square;   // 0x20
    uint32_t button_select;   // 0x24
    uint32_t button_start;    // 0x28
    uint32_t button_l;        // 0x2C
    uint32_t button_r;        // 0x30
    uint32_t button_l2;       // 0x34
    uint32_t button_r2;       // 0x38
    uint32_t button_l3;       // 0x3C
    uint32_t button_r3;       // 0x40
    uint32_t left_analog;     // 0x44
    uint32_t right_analog;    // 0x48
    uint32_t front_touch_pad; // 0x4C
    uint32_t back_touch_pad;  // 0x50
    int32_t turbo_delay;      // 0x54
    uint32_t map_port;        // 0x58
    char reserved[0x24];      // 0x5C
} ControlConfig;              // 0x80

typedef struct
{
    uint32_t version;        // 0x00
    uint32_t auto_save_load; // 0x04
    uint32_t reserved0;      // 0x08
    uint32_t reserved1;      // 0x0C
    uint32_t reserved2;      // 0x10
    char reserved[0x54];     // 0x14
} MiscConfig;                // 0x68

typedef struct
{
    uint32_t version;       // 0x00
    uint32_t preview_path;  // 0x04
    uint32_t preview_style; // 0x08
    uint32_t app_log;       // 0x0C
    uint32_t core_log;      // 0x10
    uint32_t print_log;     // 0x14
    uint32_t app_lang;      // 0x18
    char reserved[0x4C];    // 0x1C
} AppConfig;                // 0x68

extern AppConfig app_config;
extern GraphicsConfig graphics_config;
extern ControlConfig control_config;
extern MiscConfig misc_config;

extern char *private_assets_dir;
extern char *public_assets_dir;

extern ConfigList core_config_list;
extern OptionList core_option_list;
extern OverlayList setting_overlay_list;

void MakeConfigPath(char *path, char *config_name, int type);

int ResetGraphicsConfig();
int ResetControlConfig();
#if defined(WSC_BUILD)
int ResetVControlConfig();
#endif
int ResetMiscConfig();
int ResetAppConfig();

int LoadGraphicsConfig(int type);
int LoadControlConfig(int type);
int LoadMiscConfig(int type);
int LoadAppConfig(int type);

int SaveGraphicsConfig(int type);
int SaveControlConfig(int type);
int SaveMiscConfig(int type);
int SaveAppConfig(int type);

int ResetCoreConfig();
int LoadCoreConfig(int type);
int SaveCoreConfig(int type);

#endif
