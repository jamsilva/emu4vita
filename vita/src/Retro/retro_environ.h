#ifndef __M_RETRO_ENVIRON_H__
#define __M_RETRO_ENVIRON_H__

#include <libretro.h>

#include "Gui/gui.h"

#define CORE_DISK_CONTROL_INTERFACE_VERSION 1

extern struct retro_disk_control_ext_callback core_disk_control_ext_callback;
extern retro_core_options_update_display_callback_t core_options_update_display_callback;
extern enum GuiTextureFormat video_texture_format;
extern int retro_input_is_bitmasks;

#endif