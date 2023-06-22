#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/io/fcntl.h>

#include "activity/loading.h"
#include "setting/setting.h"
#include "gui/gui.h"
#include "emu/emu.h"
#include "config.h"
#include "utils.h"
#include "lang.h"

static int core_varialbe_need_update = 0;
static int core_options_display_need_update = 0;

void Retro_PushUpdateVariable()
{
    core_varialbe_need_update = 1;
}

void Retro_PushUpdateOptionsDisplay()
{
    core_options_display_need_update = 1;
}

void retroLogCallback(enum retro_log_level level, const char *fmt, ...)
{
    int enable_loading_log = (app_config.show_log && Emu_IsGameLoading());

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
        Loading_AddLog(string);

    if (!app_config.core_log)
        return;

    SceUID fd = sceIoOpen((CORE_LOG_PATH), SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd < 0)
        return;

    sceIoWrite(fd, string, strlen(string));
    sceIoClose(fd);
}

static bool setPixelFormat(enum retro_pixel_format format)
{
    core_pixel_format = format;

    switch (format)
    {
    case RETRO_PIXEL_FORMAT_0RGB1555:
    {
        core_video_pixel_format = GUI_PIXEL_FORMAT_X1U5U5U5_1RGB;
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: 0RGB1555\n");
    }
    break;
    case RETRO_PIXEL_FORMAT_RGB565:
    {
        core_video_pixel_format = GUI_PIXEL_FORMAT_U5U6U5_RGB;
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: RGB565\n");
    }
    break;
    case RETRO_PIXEL_FORMAT_XRGB8888:
    {
        core_video_pixel_format = GUI_PIXEL_FORMAT_X8U8U8U8_1RGB;
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: XRGB8888\n");
    }
    break;
    default:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: %d is unknow!\n", format);
        return false;
    }
    }

    return true;
}

static bool setSystemAvInfo(struct retro_system_av_info *info)
{
    if (!info)
        return false;

    uint32_t old_sample_rate = (uint32_t)core_system_av_info.timing.sample_rate;
    uint32_t new_sample_rate = (uint32_t)info->timing.sample_rate;

    memcpy(&core_system_av_info, info, sizeof(struct retro_system_av_info));

    if ((new_sample_rate > 0) && (new_sample_rate != old_sample_rate))
    {
        AppLog("[RETRO] frequency need reset: old %d => new %d\n", old_sample_rate, new_sample_rate);
        Emu_UpdateAudioSampleRate(new_sample_rate);
    }

    return true;
}

static bool getCurrentSoftwareFramebuffer(struct retro_framebuffer *fb)
{
    if (!fb)
        return false;

    GUI_Texture *texture = Emu_GetVideoTexture();
    if (!texture || GUI_GetTextureWidth(texture) != fb->width || GUI_GetTextureHeight(texture) != fb->height ||
        GUI_GetTextureFormat(texture) != core_video_pixel_format)
        texture = Emu_UpdateVideoTexture(fb->width, fb->height);
    if (!texture)
        return false;
    fb->format = core_pixel_format;
    fb->pitch = GUI_GetTextureStride(texture);
    fb->data = GUI_GetTextureDatap(texture);

    return true;
}

static bool setCoreOptionsUpdateDisplayCallback(struct retro_core_options_update_display_callback *cb)
{
    if (core_options_update_display_callback)
    {
        free(core_options_update_display_callback);
        core_options_update_display_callback = NULL;
    }
    if (!cb)
        return false;

    core_options_update_display_callback = malloc(sizeof(struct retro_core_options_update_display_callback));
    if (!core_options_update_display_callback)
        return false;

    memcpy(core_options_update_display_callback, cb, sizeof(struct retro_core_options_update_display_callback));

    if (core_options_display_need_update && core_options_update_display_callback->callback)
    {
        core_options_update_display_callback->callback();
        core_options_display_need_update = 0;
    }

    return true;
}

static bool setDiskControlCallback(const struct retro_disk_control_callback *cb)
{
    if (core_disk_control_ext_callback)
    {
        free(core_disk_control_ext_callback);
        core_disk_control_ext_callback = NULL;
    }
    if (!cb)
        return false;

    core_disk_control_ext_callback = malloc(sizeof(struct retro_disk_control_ext_callback));
    if (!core_disk_control_ext_callback)
        return false;
    memset(core_disk_control_ext_callback, 0, sizeof(struct retro_disk_control_ext_callback));

    core_disk_control_ext_callback->set_eject_state = cb->set_eject_state;
    core_disk_control_ext_callback->get_eject_state = cb->get_eject_state;
    core_disk_control_ext_callback->get_image_index = cb->get_image_index;
    core_disk_control_ext_callback->set_image_index = cb->set_image_index;
    core_disk_control_ext_callback->get_num_images = cb->get_num_images;
    core_disk_control_ext_callback->replace_image_index = cb->replace_image_index;
    core_disk_control_ext_callback->add_image_index = cb->add_image_index;

    return true;
}

static bool setDiskControlExtCallback(const struct retro_disk_control_ext_callback *cb)
{
    if (core_disk_control_ext_callback)
    {
        free(core_disk_control_ext_callback);
        core_disk_control_ext_callback = NULL;
    }
    if (!cb)
        return false;

    core_disk_control_ext_callback = malloc(sizeof(struct retro_disk_control_ext_callback));
    if (!core_disk_control_ext_callback)
        return false;
    memset(core_disk_control_ext_callback, 0, sizeof(struct retro_disk_control_ext_callback));

    core_disk_control_ext_callback->set_eject_state = cb->set_eject_state;
    core_disk_control_ext_callback->get_eject_state = cb->get_eject_state;
    core_disk_control_ext_callback->get_image_index = cb->get_image_index;
    core_disk_control_ext_callback->set_image_index = cb->set_image_index;
    core_disk_control_ext_callback->get_num_images = cb->get_num_images;
    core_disk_control_ext_callback->replace_image_index = cb->replace_image_index;
    core_disk_control_ext_callback->add_image_index = cb->add_image_index;

    core_disk_control_ext_callback->set_initial_image = cb->set_initial_image;
    core_disk_control_ext_callback->get_image_path = cb->get_image_path;
    core_disk_control_ext_callback->get_image_label = cb->get_image_label;

    return true;
}

bool Retro_EnvironmentCallback(unsigned int cmd, void *data)
{
    if (cmd == RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE)
    {
        *(bool *)data = core_varialbe_need_update;
        core_varialbe_need_update = 0;
        return true;
    }

    switch (cmd)
    {
    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_CAN_DUPE\n");
        if (data)
            *(bool *)data = true;
    }
    break;

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
    {
        if (!data)
            return false;
        enum retro_pixel_format format = *(const enum retro_pixel_format *)data;
        return setPixelFormat(format);
    }
    break;

    case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
    {
        if (data)
            *(const char **)data = core_assets_dir;
        CreateFolder(core_assets_dir);
    }
    break;

    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
    {
        if (data)
            *(const char **)data = core_system_dir;
        CreateFolder(core_system_dir);
    }
    break;

    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
    {
        if (data)
            *(const char **)data = CORE_SAVEFILES_DIR;
        CreateFolder(CORE_SAVEFILES_DIR);
    }
    break;

    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_LOG_INTERFACE\n");
        struct retro_log_callback *cb = (struct retro_log_callback *)data;
        if (cb)
            cb->log = retroLogCallback;
    }
    break;

    case RETRO_ENVIRONMENT_GET_LANGUAGE:
    {
        int value = GetLangValue(app_config.language);
        if (data)
            *(unsigned *)data = value;
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_LANGUAGE: %u\n", value);
    }
    break;

    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION\n");
        if (data)
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
        core_input_supports_bitmasks = 1;
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_INPUT_BITMASKS\n");
    }
    break;

    case RETRO_ENVIRONMENT_SET_ROTATION:
    {
        if (data)
            core_display_rotate = *(unsigned *)data;
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_ROTATION: %d\n", core_display_rotate);
    }
    break;

    case RETRO_ENVIRONMENT_SET_GEOMETRY:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_GEOMETRY\n");
        if (data)
            memcpy(&core_system_av_info.geometry, data, sizeof(struct retro_game_geometry));
    }
    break;

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO\n");
        struct retro_system_av_info *info = (struct retro_system_av_info *)data;
        return setSystemAvInfo(info);
    }
    break;

    case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER\n");
        struct retro_framebuffer *fb = (struct retro_framebuffer *)data;
        return getCurrentSoftwareFramebuffer(fb);
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
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_UPDATE_DISPLAY_CALLBACK\n");
        struct retro_core_options_update_display_callback *cb = (struct retro_core_options_update_display_callback *)data;
        return setCoreOptionsUpdateDisplayCallback(cb);
    }
    break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY:
    {
        // AppLog("[RETRO] RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY\n");
        const struct retro_core_option_display *option_display = (const struct retro_core_option_display *)data;
        Retro_SetCoreOptionsDisplay(option_display);
    }
    break;

    case RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_GET_DISK_CONTROL_INTERFACE_VERSION\n");
        if (data)
            *(unsigned *)data = CORE_DISK_CONTROL_INTERFACE_VERSION;
    }
    break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE\n");
        const struct retro_disk_control_callback *cb = (const struct retro_disk_control_callback *)data;
        return setDiskControlCallback(cb);
    }
    break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE:
    {
        AppLog("[RETRO] RETRO_ENVIRONMENT_SET_DISK_CONTROL_EXT_INTERFACE\n");
        const struct retro_disk_control_ext_callback *cb = (const struct retro_disk_control_ext_callback *)data;
        return setDiskControlExtCallback(cb);
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
