#ifndef __M_RETRO_OPTION_H__
#define __M_RETRO_OPTION_H__

#include <libretro.h>

#include "List/option_list.h"

#define CORE_OPTIONS_VERSION 2

int Retro_GetVariable(struct retro_variable *var);
int Retro_SetCoreOptionDisplay(const struct retro_core_option_display *option_display);
int Retro_GetOptionListFromVariables(struct retro_variable *varialbes);
int Retro_GetOptionListFromOptions(void *options, int type);

#endif