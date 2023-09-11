#ifndef __M_RETRO_H__
#define __M_RETRO_H__

#include <psp2/display.h>
#include <libretro.h>

#include "gui/gui.h"
#include "retro_environ.h"
#include "retro_option.h"
#include "file.h"

#define CORE_OPTIONS_VERSION 2
#define CORE_DISK_CONTROL_INTERFACE_VERSION 1

extern char core_assets_dir[MAX_PATH_LENGTH];
extern char core_system_dir[MAX_PATH_LENGTH];

extern struct retro_core_options_update_display_callback *core_options_update_display_callback;
extern struct retro_disk_control_ext_callback *core_disk_control_ext_callback;

extern struct retro_system_info core_system_info;
extern struct retro_system_av_info core_system_av_info;
extern enum retro_pixel_format core_pixel_format;
extern GuiPixelFormat core_video_pixel_format;
extern int core_display_rotate;
extern int core_input_supports_bitmasks;

int Retro_InitLib();
int Retro_DeinitLib();

void Retro_SetControllerPortDevices();

#endif