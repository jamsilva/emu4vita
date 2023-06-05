#ifndef __M_RETRO_ENVIRON_H__
#define __M_RETRO_ENVIRON_H__

#include <libretro.h>

#include "Gui/gui.h"

extern retro_core_options_update_display_callback_t core_options_update_display_callback;
extern enum GuiTextureFormat video_texture_format;
extern int retro_input_is_bitmasks;

#endif