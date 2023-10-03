#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__

#include <stdint.h>

#include "list/config_list.h"
#include "list/option_list.h"
#include "list/overlay_list.h"
#include "utils_string.h"
#include "file.h"

#define APP_CONFIG_VERSION 2      // v1.01
#define GRAPHICS_CONFIG_VERSION 2 // v1.01
#define CONTROL_CONFIG_VERSION 2  // v1.01
#define CORE_CONFIG_VERSION 1     // v1.00
#define HOTKEY_CONFIG_VERSION 1   // v1.00
#define MISC_CONFIG_VERSION 1     // v1.00

#define APP_CONFIG_NAME "app.cfg"
#define GRAPHICS_CONFIG_NAME "graphics.cfg"
#define CONTROL_CONFIG_NAME "control.cfg"
#define HOTKEY_CONFIG_NAME "hotkey.cfg"
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
#define BITMASK_RETRO_KEY(id) (1 << id)

#define ENABLE_BITMASK_RETRO_KEY(id) ((1 << id) | ENABLE_KEY_BITMASK)
#define ENABLE_TURBO_BITMASK_RETRO_KEY(id) ((1 << id) | ENABLE_KEY_BITMASK | TURBO_KEY_BITMASK)

enum TypeDisplaySize
{
    TYPE_DISPLAY_SIZE_1X,
    TYPE_DISPLAY_SIZE_2X,
    TYPE_DISPLAY_SIZE_3X,
    TYPE_DISPLAY_SIZE_FULL,
};

enum TypeDisplayRatio
{
    TYPE_ASPECT_RATIO_DEFAULT,
    TYPE_ASPECT_RATIO_BY_GAME_RESOLUTION,
    TYPE_ASPECT_RATIO_BY_DEV_SCREEN,
    TYPE_ASPECT_RATIO_8_7,
    TYPE_ASPECT_RATIO_4_3,
    TYPE_ASPECT_RATIO_3_2,
    TYPE_ASPECT_RATIO_16_9,
};

enum TypeDisplayRotate
{
    TYPE_DISPLAY_ROTATE_DISABLE,
    TYPE_DISPLAY_ROTATE_CW_90,
    TYPE_DISPLAY_ROTATE_CW_180,
    TYPE_DISPLAY_ROTATE_CW_270,
    TYPE_DISPLAY_ROTATE_DEFAULT,
};

enum TypeGraphicsFiltering
{
    TYPE_GRAPHICS_SHADER_DEFAULT,
    TYPE_GRAPHICS_SHADER_LCD3X,
    TYPE_GRAPHICS_SHADER_SHARP_BILINEAR_SIMPLE,
    TYPE_GRAPHICS_SHADER_SHARP_BILINEAR,
    TYPE_GRAPHICS_SHADER_ADVANCED_AA,
};

enum TypePreviewPath
{
    TYPE_PREVIEW_PATH_AUTO,
    TYPE_PREVIEW_PATH_DEFAULT,
    TYPE_PREVIEW_PATH_SAVESTATE,
};

enum TypePreviewStyle
{
    TYPE_PREVIEW_SCALE_TYPE_FIT_CENTER_INSIDE,
    TYPE_PREVIEW_SCALE_TYPE_FIT_XY,
    TYPE_PREVIEW_SCALE_TYPE_FIT_CENTER_CROP,
};

enum TypeConfig
{
    TYPE_CONFIG_MAIN,
    TYPE_CONFIG_GAME,
};

typedef struct
{
    uint32_t version;         // 0x00
    uint32_t display_size;    // 0x04
    uint32_t aspect_ratio;    // 0x08
    uint32_t display_rotate;  // 0x0C
    uint32_t graphics_shader; // 0x10
    uint32_t graphics_smooth; // 0x14
    uint32_t overlay_select;  // 0x18
    uint32_t overlay_mode;    // 0x1C
    uint32_t show_fps;        // 0x20
    char reserved[0x5C];      // 0x24
} GraphicsConfig;             // 0x80

typedef struct
{
    uint32_t version;            // 0x00
    uint32_t button_up;          // 0x04
    uint32_t button_down;        // 0x08
    uint32_t button_left;        // 0x0C
    uint32_t button_right;       // 0x10
    uint32_t button_triangle;    // 0x14
    uint32_t button_circle;      // 0x18
    uint32_t button_cross;       // 0x1C
    uint32_t button_square;      // 0x20
    uint32_t button_select;      // 0x24
    uint32_t button_start;       // 0x28
    uint32_t button_l;           // 0x2C
    uint32_t button_r;           // 0x30
    uint32_t button_l2;          // 0x34
    uint32_t button_r2;          // 0x38
    uint32_t button_l3;          // 0x3C
    uint32_t button_r3;          // 0x40
    uint32_t left_analog_left;   // 0x44
    uint32_t left_analog_up;     // 0x48
    uint32_t left_analog_right;  // 0x4C
    uint32_t left_analog_down;   // 0x50
    uint32_t right_analog_left;  // 0x54
    uint32_t right_analog_up;    // 0x58
    uint32_t right_analog_right; // 0x5C
    uint32_t right_analog_down;  // 0x60
    uint32_t front_touch_pad;    // 0x64
    uint32_t back_touch_pad;     // 0x68
    int32_t turbo_delay;         // 0x6C
    uint32_t ctrl_player;        // 0x70
    char reserved[0x0C];         // 0x74
} ControlConfig;                 // 0x80

typedef struct
{
    uint32_t version;        // 0x00
    uint32_t hk_loadstate;   // 0x04
    uint32_t hk_savestate;   // 0x08
    uint32_t hk_speed_up;    // 0x0C
    uint32_t hk_speed_down;  // 0x10
    uint32_t hk_player_up;   // 0x14
    uint32_t hk_player_down; // 0x18
    uint32_t hk_exit_game;   // 0x1C
    char reserved[0x60];     // 0x20
} HotkeyConfig;              // 0x80

typedef struct
{
    uint32_t version;        // 0x00
    uint32_t auto_save_load; // 0x04
    char reserved[0x78];     // 0x08
} MiscConfig;                // 0x80

typedef struct
{
    uint32_t version;       // 0x00
    uint32_t preview_path;  // 0x04
    uint32_t preview_style; // 0x08
    uint32_t app_log;       // 0x0C
    uint32_t core_log;      // 0x10
    uint32_t show_log;      // 0x14
    uint32_t language;      // 0x18
    char reserved[0x64];    // 0x1C
} AppConfig;                // 0x80

extern AppConfig app_config;
extern GraphicsConfig graphics_config;
extern ControlConfig control_config;
extern HotkeyConfig hotkey_config;
extern MiscConfig misc_config;

extern char *private_assets_dir;
extern char *public_assets_dir;

extern LinkedList *core_option_list;
extern LinkedList *graphics_overlay_list;

void MakeConfigPath(char *path, char *config_name, int type);

int ResetGraphicsConfig();
int ResetControlConfig();
int ResetHotkeyConfig();
int ResetMiscConfig();
int ResetAppConfig();

int LoadGraphicsConfig(int type);
int LoadControlConfig(int type);
int LoadHotkeyConfig(int type);
int LoadMiscConfig(int type);
int LoadAppConfig(int type);

int SaveGraphicsConfig(int type);
int SaveControlConfig(int type);
int SaveHotkeyConfig(int type);
int SaveMiscConfig(int type);
int SaveAppConfig(int type);

int ResetCoreConfig();
int LoadCoreConfig(int type);
int SaveCoreConfig(int type);

#endif
