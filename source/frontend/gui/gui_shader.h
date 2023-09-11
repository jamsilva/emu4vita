#ifndef __M_GUI_SHADER_H__
#define __M_GUI_SHADER_H__

#include "gui.h"

extern GUI_Shader *lcd3x_shader, *sharp_bilinear_simple_shader, *sharp_bilinear_shader, *advanced_aa_shader;

void GUI_InitShaders();
void GUI_DeinitShaders();

#endif