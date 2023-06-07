#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "List/config_list.h"
#include "List/option_list.h"
#include "List/overlay_list.h"
#include "Activity/browser.h"
#include "Setting/setting.h"
#include "Retro/retro.h"
#include "config.h"
#include "file.h"
#include "utils.h"

AppConfig app_config;
MiscConfig misc_config;
GraphicsConfig graphics_config;
ControlConfig control_config;

char *private_assets_dir = NULL;
char *public_assets_dir = NULL;

ConfigList core_config_list = {0};
OptionList core_option_list = {0};
OverlayList setting_overlay_list = {0};

void MakeConfigPath(char *path, char *config_name, int type)
{
    if (type == TYPE_CONFIG_GAME)
    {
        char name[MAX_NAME_LENGTH];
        MakeCurrentFileName(name);
        char base_name[MAX_NAME_LENGTH];
        MakeBaseName(base_name, name, MAX_NAME_LENGTH);
        snprintf(path, MAX_PATH_LENGTH, "%s/%s/%s", (CORE_CONFIGS_DIR), base_name, config_name);
    }
    else
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", (APP_DATA_DIR), config_name);
    }
}

int ResetGraphicsConfig()
{
    memset(&graphics_config, 0, sizeof(GraphicsConfig));
    graphics_config.version = GRAPHICS_CONFIG_VERSION;
    graphics_config.display_size = TYPE_DISPLAY_SIZE_FULL;
    graphics_config.aspect_ratio = TYPE_DISPLAY_RATIO_DEFAULT;
    graphics_config.display_rotate = 0;
    graphics_config.graphics_filtering = 0;
    graphics_config.graphics_smooth = 0;
    graphics_config.overlay_select = 0;
    graphics_config.overlay_mode = 0;
    graphics_config.show_fps = 0;

    return 0;
}

int ResetControlConfig()
{
    memset(&control_config, 0, sizeof(ControlConfig));
    control_config.version = CONTROL_CONFIG_VERSION;
    control_config.map_port = 0;

    control_config.button_up = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_UP);
    control_config.button_down = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_DOWN);
    control_config.button_left = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_LEFT);
    control_config.button_right = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_RIGHT);
#if defined(FC_BUILD) || defined(GBC_BUILD) || defined(NGP_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_square = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_l = 0;
    control_config.button_r = 0;
#elif defined(SFC_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
#elif defined(GBA_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_square = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
#elif defined(MD_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
#elif defined(WSC_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_square = GET_BITMASK_KEY_TURBO(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_l = 0;
    control_config.button_r = 0;
#elif defined(PCE_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
#elif defined(ARC_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
    control_config.button_l2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L2);
    control_config.button_r2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R2);
#elif defined(MRP_BUILD)
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
    control_config.button_l2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L2);
    control_config.button_r2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R2);
    control_config.button_l3 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L3);
    control_config.button_r3 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R3);
#else
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_A);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_B);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_X);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_Y);
    control_config.button_l = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_r = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
    control_config.button_l2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L2);
    control_config.button_r2 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R2);
    control_config.button_l3 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L3);
    control_config.button_r3 = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R3);
#endif
#if defined(WSC_BUILD)
    control_config.button_select = 0;
#else
    control_config.button_select = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_SELECT);
#endif
    control_config.button_start = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_START);
    control_config.left_analog = 1;
    control_config.right_analog = 0;
#if defined(PS_BUILD)
    control_config.front_touch_pad = 1;
    control_config.back_touch_pad = 1;
#else
#if defined(ARC_BUILD)
    control_config.front_touch_pad = 1;
#else
    control_config.front_touch_pad = 0;
#endif
    control_config.back_touch_pad = 0;
#endif

    control_config.turbo_delay = 5;
#if defined(WSC_BUILD)
    graphics_config.display_rotate = 0;
#endif

    return 0;
}

#if defined(WSC_BUILD)
int ResetVControlConfig()
{
    memset(&control_config, 0, sizeof(ControlConfig));
    control_config.version = CONTROL_CONFIG_VERSION;
    control_config.map_port = 0;
    control_config.button_left = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R2);
    control_config.button_up = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_R);
    control_config.button_right = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L2);
    control_config.button_down = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_L);
    control_config.button_square = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_UP);
    control_config.button_triangle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_RIGHT);
    control_config.button_circle = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_DOWN);
    control_config.button_cross = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_LEFT);
    control_config.button_l = 0;
    control_config.button_r = 0;
    control_config.button_select = 0;
    control_config.button_start = GET_BITMASK_KEY(RETRO_DEVICE_ID_JOYPAD_START);
    control_config.left_analog = 1;
    control_config.right_analog = 0;
    control_config.front_touch_pad = 0;
    control_config.back_touch_pad = 0;
    control_config.turbo_delay = 5;
    graphics_config.display_rotate = 3;

    return 0;
}
#endif

int ResetMiscConfig()
{
    memset(&misc_config, 0, sizeof(MiscConfig));
    misc_config.version = MISC_CONFIG_VERSION;
    misc_config.auto_save_load = 1;

    return 0;
}

int ResetAppConfig()
{
    memset(&app_config, 0, sizeof(AppConfig));
    app_config.version = APP_CONFIG_VERSION;
    app_config.preview_path = TYPE_PREVIEW_PATH_AUTO;
    app_config.preview_style = TYPE_PREVIEW_STYLE_PRESERVE_FULL;
    app_config.app_log = 1;
    app_config.core_log = 0;
#if defined(FBA_BUILD)
    app_config.print_log = 1;
#else
    app_config.print_log = 0;
#endif
    app_config.lang = 1;

    return 0;
}

int LoadGraphicsConfig(int type)
{
    GraphicsConfig config;
    memset(&config, 0, sizeof(GraphicsConfig));

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, GRAPHICS_CONFIG_NAME, type);

    int overlay_select = graphics_config.overlay_select;

    int ret = ReadFile(path, &config, sizeof(GraphicsConfig));
    if (ret < 0 || ret != sizeof(GraphicsConfig) || config.version != GRAPHICS_CONFIG_VERSION)
    {
        if (type == TYPE_CONFIG_MAIN)
            ResetGraphicsConfig();
        return -1;
    }

    memcpy(&graphics_config, &config, sizeof(GraphicsConfig));

    if (graphics_config.overlay_select > setting_overlay_list.length)
        graphics_config.overlay_select = setting_overlay_list.length;

    return 0;
}

int LoadControlConfig(int type)
{
    ControlConfig config;
    memset(&config, 0, sizeof(ControlConfig));

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, CONTROL_CONFIG_NAME, type);

    int ret = ReadFile(path, &config, sizeof(ControlConfig));
    if (ret < 0 || ret != sizeof(ControlConfig) || config.version != CONTROL_CONFIG_VERSION)
    {
        if (type == TYPE_CONFIG_MAIN)
            ResetControlConfig();
        return -1;
    }

    memcpy(&control_config, &config, sizeof(ControlConfig));

    return 0;
}

int LoadMiscConfig(int type)
{
    MiscConfig config;
    memset(&config, 0, sizeof(MiscConfig));

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, MISC_CONFIG_NAME, type);

    int ret = ReadFile(path, &config, sizeof(MiscConfig));
    if (ret < 0 || ret != sizeof(MiscConfig) || config.version != MISC_CONFIG_VERSION)
    {
        if (type == TYPE_CONFIG_MAIN)
            ResetMiscConfig();
        return -1;
    }

    memcpy(&misc_config, &config, sizeof(MiscConfig));

    return 0;
}

int LoadAppConfig(int type)
{
    AppConfig config;
    memset(&config, 0, sizeof(AppConfig));

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, APP_CONFIG_NAME, type);

    int ret = ReadFile(path, &config, sizeof(AppConfig));
    if (ret < 0 || ret != sizeof(AppConfig) || config.version != APP_CONFIG_VERSION)
    {
        if (type == TYPE_CONFIG_MAIN)
            ResetAppConfig();
        return -1;
    }

    memcpy(&app_config, &config, sizeof(AppConfig));

    return 0;
}

int SaveGraphicsConfig(int type)
{
    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, GRAPHICS_CONFIG_NAME, type);

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    return WriteFile(path, &graphics_config, sizeof(GraphicsConfig));
}

int SaveControlConfig(int type)
{
    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, CONTROL_CONFIG_NAME, type);

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    return WriteFile(path, &control_config, sizeof(ControlConfig));
}

int SaveMiscConfig(int type)
{
    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, MISC_CONFIG_NAME, type);

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    return WriteFile(path, &misc_config, sizeof(MiscConfig));
}

int SaveAppConfig(int type)
{
    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, APP_CONFIG_NAME, type);

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    return WriteFile(path, &app_config, sizeof(AppConfig));
}

int ResetCoreConfig()
{
    if (core_option_list.length <= 0)
        return -1;

    OptionListEntry *option_entry = core_option_list.head;

    while (option_entry)
    {
        if (option_entry->key)
        {
            option_entry->sel_pos = 0;

            int i;
            for (i = 0; i < option_entry->n_values; i++)
            {
                if (strcmp(option_entry->values[i].value, option_entry->default_value) == 0)
                {
                    option_entry->sel_pos = i;
                    break;
                }
            }
        }
        option_entry = option_entry->next;
    }

    return 0;
}

int LoadCoreConfig(int type)
{
    if (core_option_list.length <= 0)
        return -1;

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, CORE_CONFIG_NAME, type);

    ConfigListEmpty(&core_config_list);
    ConfigListGetEntries(&core_config_list, path);

    if (core_config_list.length <= 0)
        return -1;

    ConfigListEntry *config_entry;
    OptionListEntry *option_entry = core_option_list.head;

    while (option_entry)
    {
        if (option_entry->key)
        {
            config_entry = core_config_list.head;
            while (config_entry)
            {
                if (strcmp(option_entry->key, config_entry->key) == 0)
                {
                    int i;
                    for (i = 0; i < option_entry->n_values; i++)
                    {
                        if (strcmp(option_entry->values[i].value, config_entry->value) == 0)
                        {
                            option_entry->sel_pos = i;
                            // printf("Option get default value: %s\n", config_entry->value);
                            break;
                        }
                    }
                    break;
                }
                config_entry = config_entry->next;
            }
        }
        option_entry = option_entry->next;
    }

    return 0;
}

int SaveCoreConfig(int type)
{
    if (core_option_list.length <= 0)
        return -1;

    char path[MAX_PATH_LENGTH];
    MakeConfigPath(path, CORE_CONFIG_NAME, type);

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    int ret = OptionListSaveConfig(&core_option_list, path);
    return ret;
}
