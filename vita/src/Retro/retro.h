#ifndef __M_RETRO_H__
#define __M_RETRO_H__

#include <psp2/display.h>
#include <libretro.h>

#include "Gui/gui.h"
#include "retro_environ.h"
#include "retro_option.h"
#include "file.h"

extern struct retro_system_info core_system_info;
extern struct retro_system_av_info core_system_av_info;
extern enum GuiTextureFormat video_texture_format;

extern char core_assets_dir[MAX_PATH_LENGTH];
extern char core_system_dir[MAX_PATH_LENGTH];

extern unsigned core_option_language;

int MakeSrmPath(char *path);

int Retro_InitLib();
int Retro_DeinitLib();

void Retro_SetControllerPortDevices();

void Retro_RequestUpdateVariable();
void Retro_UpdateCoreOptionsDisplay();

#endif