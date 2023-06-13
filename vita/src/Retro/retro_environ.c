#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/io/fcntl.h>

#include "Setting/setting.h"
#include "Emu/emu.h"
#include "Gui/gui.h"
#include "retro.h"
#include "config.h"
#include "utils.h"
#include "log_scr.h"
#include "lang.h"

struct retro_disk_control_ext_callback core_disk_control_ext_callback = {0};
retro_core_options_update_display_callback_t core_options_update_display_callback = NULL;
enum GuiTextureFormat video_texture_format = GUI_TEXTURE_FORMAT_U5U6U5_RGB;
int retro_input_is_bitmasks = 0;

static int retro_varialbe_need_update = 0;
static int retro_option_display_need_update = 0;

void Retro_RequestUpdateVariable()
{
    retro_varialbe_need_update = 1;
}

void Retro_UpdateCoreOptionsDisplay()
{
    if (core_options_update_display_callback)
    {
        core_options_update_display_callback();
        Setting_RequestRefreshOptionDisplay();
    }
    else
    {
        retro_option_display_need_update = 1;
    }
}

void Retro_LogCallback(enum retro_log_level level, const char *fmt, ...)
{
    int enable_loading_log = (app_config.print_log && Emu_IsGameLoading());

    if (!app_config.core_log && !enable_loading_log)
        return;

    va_list list;
    char string[512];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);

#ifdef DEBUG
    printf(string);
#endif

    if (enable_loading_log)
        LogScrPrintText(string);

    if (!app_config.core_log)
        return;

    SceUID fd = sceIoOpen((CORE_LOG_PATH), SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd < 0)
        return;

    sceIoWrite(fd, string, strlen(string));
    sceIoClose(fd);
}

void setDiskControlCallback(const struct retro_disk_control_callback *cb)
{
    memset(&core_disk_control_ext_callback, 0, sizeof(struct retro_disk_control_ext_callback));

    if (!cb)
        return;

    core_disk_control_ext_callback.set_eject_state = cb->set_eject_state;
    core_disk_control_ext_callback.get_eject_state = cb->get_eject_state;
    core_disk_control_ext_callback.get_image_index = cb->get_image_index;
    core_disk_control_ext_callback.set_image_index = cb->set_image_index;
    core_disk_control_ext_callback.get_num_images = cb->get_num_images;
    core_disk_control_ext_callback.replace_image_index = cb->replace_image_index;
    core_disk_control_ext_callback.add_image_index = cb->add_image_index;
}

void setDiskControlExtCallback(const struct retro_disk_control_ext_callback *cb)
{
    memset(&core_disk_control_ext_callback, 0, sizeof(struct retro_disk_control_ext_callback));

    if (!cb)
        return;

    core_disk_control_ext_callback.set_eject_state = cb->set_eject_state;
    core_disk_control_ext_callback.get_eject_state = cb->get_eject_state;
    core_disk_control_ext_callback.get_image_index = cb->get_image_index;
    core_disk_control_ext_callback.set_image_index = cb->set_image_index;
    core_disk_control_ext_callback.get_num_images = cb->get_num_images;
    core_disk_control_ext_callback.replace_image_index = cb->replace_image_index;
    core_disk_control_ext_callback.add_image_index = cb->add_image_index;

    core_disk_control_ext_callback.set_initial_image = cb->set_initial_image;
    core_disk_control_ext_callback.get_image_path = cb->get_image_path;
    core_disk_control_ext_callback.get_image_label = cb->get_image_label;
}

bool Retro_EnvironmentCallback(unsigned int cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE)
    {
        *(bool *)data = retro_varialbe_need_update;
        if (retro_varialbe_need_update)
            retro_varialbe_need_update = 0;
        return true;
    }

    switch (cmd)
    {
    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_CAN_DUPE: 1\n");
        *(bool *)data = true;
    }
    break;

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
    {
        enum retro_pixel_format pix_fmt = *(const enum retro_pixel_format *)data;
        enum GuiTextureFormat texture_format = video_texture_format;
        switch (pix_fmt)
        {
        case RETRO_PIXEL_FORMAT_0RGB1555:
        {
            texture_format = GUI_TEXTURE_FORMAT_X1U5U5U5_1RGB;
            AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: 0RGB1555\n");
        }
        break;

        case RETRO_PIXEL_FORMAT_RGB565:
        {
            texture_format = GUI_TEXTURE_FORMAT_U5U6U5_RGB;
            AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: RGB565\n");
        }
        break;

        case RETRO_PIXEL_FORMAT_XRGB8888:
        {
            texture_format = GUI_TEXTURE_FORMAT_X8U8U8U8_1RGB;
            AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: XRGB8888\n");
        }
        break;

        default:
        {
            AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: %d is unknow!\n", pix_fmt);
            return false;
        }
        }
        if (texture_format != video_texture_format)
        {
            AppLog("[RETRO] pixel_format is changed, video will refresh!\n");
            video_texture_format = texture_format;
            Emu_RequestRefreshVideo(1);
        }
    }
    break;

    case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
    {
        *(const char **)data = core_assets_dir;
        CreateFolder(core_assets_dir);
    }
    break;

    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    {
        *(const char **)data = core_system_dir;
        CreateFolder(core_system_dir);
    }
    break;

    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    {
        *(const char **)data = CORE_SAVEFILES_DIR;
        CreateFolder(CORE_SAVEFILES_DIR);
    }
    break;

    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_LOG_INTERFACE\n");
        struct retro_log_callback *cb = (struct retro_log_callback *)data;
        if (cb)
            cb->log = Retro_LogCallback;
    }
    break;

    case RETRO_ENVIRONMENT_GET_LANGUAGE:
    {
        int value = GetLangValue(app_config.app_lang);
        *(unsigned *)data = value;
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_LANGUAGE: %u\n", value);
    }
    break;

    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION\n");
        *(unsigned *)data = CORE_OPTIONS_VERSION;
    }
    break;

    case RETRO_ENVIRONMENT_GET_VARIABLE:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_VARIABLE\n");
        struct retro_variable *var = (struct retro_variable *)data;
        Retro_GetVariable(var);
    }
    break;

    case RETRO_ENVIRONMENT_SET_VARIABLES:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_VARIABLES\n");
        Retro_GetOptionListFromVariables(data);
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS\n");
        Retro_GetOptionListFromOptions(data, RETRO_ENVIRONMENT_SET_CORE_OPTIONS);
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL\n");
        Retro_GetOptionListFromOptions(data, RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL);
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2\n");
        Retro_GetOptionListFromOptions(data, RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2);

        return false;
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL\n");
        Retro_GetOptionListFromOptions(data, RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL);

        return false;
    }
    break;

    case RETRO_ENVIRONMENT_GET_INPUT_BITMASKS:
    {
        retro_input_is_bitmasks = 1;
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_INPUT_BITMASKS\n");
    }
    break;

    case RETRO_ENVIRONMENT_SET_ROTATION:
    {
        unsigned rotation = *(unsigned *)data;
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_ROTATION: %d\n", rotation);
        AppLog("[RETRO] do nothing, use app's rotation\n");
    }
    break;

    case RETRO_ENVIRONMENT_SET_GEOMETRY:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_GEOMETRY\n");
        struct retro_system_av_info *av_info = &core_system_av_info;
        struct retro_game_geometry *geom = (struct retro_game_geometry *)&av_info->geometry;
        const struct retro_game_geometry *in_geom = (const struct retro_game_geometry *)data;
        if (!geom)
            return false;
        if (in_geom)
        {
            memcpy(geom, in_geom, sizeof(struct retro_game_geometry));
            Emu_RequestRefreshVideo(1);
        }
    }
    break;

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO\n");
        struct retro_system_av_info *av_info = (struct retro_system_av_info *)data;
        uint32_t old_sample_rate = (uint32_t)core_system_av_info.timing.sample_rate;
        uint32_t new_sample_rate = (uint32_t)av_info->timing.sample_rate;
        memcpy(&core_system_av_info, av_info, sizeof(struct retro_system_av_info));
        if ((new_sample_rate > 0) && (new_sample_rate != old_sample_rate))
        {
            AppLog("[RETRO] frequency need reset: old %d => new %d\n", old_sample_rate, new_sample_rate);
            Emu_UpdateAudioSampleRate(new_sample_rate);
        }
        Emu_RequestRefreshVideo(1);
    }
    break;

    case RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE\n");
        int result = 1 | 2;
        if (data)
            *(int *)data = result;
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK:
    {
        const struct retro_core_options_update_display_callback
            *update_display_cb =
                (const struct retro_core_options_update_display_callback *)data;

        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK\n");
        core_options_update_display_callback = NULL;
        if (update_display_cb && update_display_cb->callback)
        {
            core_options_update_display_callback = update_display_cb->callback;
            if (retro_option_display_need_update)
            {
                core_options_update_display_callback();
                retro_option_display_need_update = 0;
            }
        }
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY\n");
        const struct retro_core_option_display
            *option_display =
                (const struct retro_core_option_display *)data;

        if (option_display && option_display->key)
            Retro_SetCoreOptionDisplay(option_display);
    }
    break;

    case RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION\n");
        *(unsigned *)data = CORE_DISK_CONTROL_INTERFACE_VERSION;
    }
    break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
    {
        const struct retro_disk_control_callback *control_cb =
            (const struct retro_disk_control_callback *)data;

        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE\n");
        setDiskControlCallback(control_cb);
    }
    break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE:
    {
        const struct retro_disk_control_ext_callback *control_cb =
            (const struct retro_disk_control_ext_callback *)data;

        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE\n");
        setDiskControlExtCallback(control_cb);
    }
    break;

    default:
    {
        // printf("[Environ] UNSUPPORTED (#%u).\n", cmd);
        return false;
    }
    }

    return true;
}
