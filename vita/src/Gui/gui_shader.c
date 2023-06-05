#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <vita2d.h>
#include <vita2d_ext.h>

#include "gui.h"

#include "shader/texture_v.h"
#include "shader/texture_f.h"
#include "shader/sharp_bilinear_v.h"
#include "shader/sharp_bilinear_f.h"
#include "shader/sharp_bilinear_simple_f.h"
#include "shader/sharp_bilinear_simple_v.h"
#include "shader/lcd3x_v.h"
#include "shader/lcd3x_f.h"
#include "shader/advanced_aa_v.h"
#include "shader/advanced_aa_f.h"

GUI_Shader *lcd3x_shader = NULL;
GUI_Shader *sharp_bilinear_simple_shader = NULL;
GUI_Shader *sharp_bilinear_shader = NULL;
GUI_Shader *advanced_aa_shader = NULL;

void GUI_InitShaders()
{
    lcd3x_shader = vita2d_create_shader((SceGxmProgram *)lcd3x_v, (SceGxmProgram *)lcd3x_f);
    sharp_bilinear_simple_shader = vita2d_create_shader((SceGxmProgram *)sharp_bilinear_simple_v, (SceGxmProgram *)sharp_bilinear_simple_f);
    sharp_bilinear_shader = vita2d_create_shader((SceGxmProgram *)sharp_bilinear_v, (SceGxmProgram *)sharp_bilinear_f);
    advanced_aa_shader = vita2d_create_shader((SceGxmProgram *)advanced_aa_v, (SceGxmProgram *)advanced_aa_f);
}

void GUI_DeinitShaders()
{
    if (lcd3x_shader)
        vita2d_free_shader(lcd3x_shader);
    lcd3x_shader = NULL;
    if (sharp_bilinear_simple_shader)
        vita2d_free_shader(sharp_bilinear_simple_shader);
    sharp_bilinear_simple_shader = NULL;
    if (sharp_bilinear_shader)
        vita2d_free_shader(sharp_bilinear_shader);
    sharp_bilinear_shader = NULL;
    if (advanced_aa_shader)
        vita2d_free_shader(advanced_aa_shader);
    advanced_aa_shader = NULL;
}
