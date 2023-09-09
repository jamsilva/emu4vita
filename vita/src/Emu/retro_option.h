#ifndef __M_RETRO_OPTION_H__
#define __M_RETRO_OPTION_H__

#include <libretro.h>

int Retro_GetVariable(struct retro_variable *var);

int Retro_SetCoreOptionsDisplay(const struct retro_core_option_display *option_display);
void Retro_UpdateCoreOptionsDisplay();

int Retro_SetOptionListFromVariables(struct retro_variable *varialbes);
int Retro_SetOptionListFromOptions(void *options, int type);

#endif