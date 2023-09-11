#ifndef VITA2D_EXT_H
#define VITA2D_EXT_H

#include <psp2/gxm.h>
#include <psp2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vita2d_shader {
  SceGxmShaderPatcherId vertexProgramId;
  SceGxmShaderPatcherId fragmentProgramId;
  SceGxmVertexProgram *vertexProgram;
  SceGxmFragmentProgram *fragmentProgram;
  const SceGxmProgramParameter * paramPositionAttribute;
  const SceGxmProgramParameter * paramTexcoordAttribute;
  const SceGxmProgramParameter * wvpParam;
} vita2d_shader;

int vita2d_ext_init(SceGxmContext *context, SceGxmShaderPatcher *shaderPatcher);
int vita2d_ext_fini();

vita2d_shader *vita2d_create_shader(const SceGxmProgram* vertexProgramGxp, const SceGxmProgram* fragmentProgramGxp);
void vita2d_free_shader(vita2d_shader* shader);
void vita2d_set_shader(const vita2d_shader *shader);
void vita2d_set_wvp_uniform(const vita2d_shader *shader, float *matrix);
void vita2d_set_vertex_uniform(const vita2d_shader *shader, const char * param, const float *value, unsigned int length);
void vita2d_set_fragment_uniform(const vita2d_shader *shader, const char * param, const float *value, unsigned int length);
void vita2d_draw_texture_part_scale_rotate_generic(const vita2d_texture *texture, float x, float y,
	float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale, float rad);

#ifdef __cplusplus
}
#endif

#endif
