#ifndef __M_GUI_LIB_H__
#define __M_GUI_LIB_H__

#include <vita2d.h>
#include <vita2d_ext.h>

typedef vita2d_texture GUI_Texture;
typedef vita2d_shader GUI_Shader;

typedef struct GUI_Rect
{
    int x, y;
    int w, h;
} GUI_Rect;

//------------------------------------------------------------------------------------------------------------------------------------------

int GUI_InitLib();
int GUI_DeinitLib();

//------------------------------------------------------------------------------------------------------------------------------------------

// Renderer
void GUI_StartDrawing();
void GUI_EndDrawing();
void GUI_SetRendertarget(GUI_Texture *texture);
#define GUI_SetVblankWait(enable) vita2d_set_vblank_wait(enable)
#define GUI_WaitRenderingDone vita2d_wait_rendering_done

int GUI_EnableClipping(int x, int y, int w, int h);
int GUI_DisableClipping();

// Draw text
int GUI_GetFontSize();
void GUI_SetFontSize(int size);

int GUI_DrawText(int x, int y, unsigned int color, const char *text);
int GUI_DrawTextf(int x, int y, unsigned int color, const char *text, ...);
int GUI_GetTextWidth(const char *text);
int GUI_GetTextHeight(const char *text);

// Draw geometric figure
#define GUI_DrawPixel(x, y, color) vita2d_draw_pixel(x, y, color)
#define GUI_DrawLine(x0, y0, x1, y1, color) vita2d_draw_line(x, y, color)
#define GUI_DrawFillRectangle(x, y, w, h, color) vita2d_draw_rectangle(x, y, w, h, color)
void GUI_DrawEmptyRectangle(float x, float y, float w, float h, float line_size, unsigned int color);
#define GUI_DrawFillCircle(x, y, radius, color) vita2d_draw_fill_circle(x, y, radius, color)

// Image
#define GUI_LoadPNGFile(path) vita2d_load_PNG_file(path)
#define GUI_LoadPNGBuffer(buffer) vita2d_load_PNG_buffer(buffer)
#define GUI_LoadJPEGFile(path) vita2d_load_JPEG_file(path)
#define GUI_LoadJPEGBuffer(buffer, buffer_size) vita2d_load_JPEG_buffer(buffer, buffer_size)
#define GUI_LoadBMPFile(path) vita2d_load_BMP_file(path)
#define GUI_LoadBMPBuffer(buffer) vita2d_load_BMP_buffer(buffer)

// Texture
typedef SceGxmTextureFormat GuiPixelFormat;
#define GUI_PIXEL_FORMAT_U8U8U8U8_ABGR SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR
#define GUI_PIXEL_FORMAT_X8U8U8U8_1RGB SCE_GXM_TEXTURE_FORMAT_X8U8U8U8_1RGB
#define GUI_PIXEL_FORMAT_U5U6U5_RGB SCE_GXM_TEXTURE_FORMAT_U5U6U5_RGB
#define GUI_PIXEL_FORMAT_X1U5U5U5_1RGB SCE_GXM_TEXTURE_FORMAT_X1U5U5U5_1RGB

typedef SceGxmTextureFilter GuiTextureFilter;
#define GUI_TEXTURE_FILTER_POINT SCE_GXM_TEXTURE_FILTER_POINT
#define GUI_TEXTURE_FILTER_LINEAR SCE_GXM_TEXTURE_FILTER_LINEAR

#define GUI_CreatTexture vita2d_create_empty_texture(w, h)
#define GUI_CreatTextureFormat(w, h, format) vita2d_create_empty_texture_format(w, h, format)
#define GUI_CreatTextureRendertarget(w, h, format) vita2d_create_empty_texture_rendertarget(w, h, format)
#define GUI_DestroyTexture(texture) vita2d_free_texture(texture)

#define GUI_GetTextureWidth(texture) vita2d_texture_get_width(texture)
#define GUI_GetTextureHeight(texture) vita2d_texture_get_height(texture)
#define GUI_GetTextureStride(texture) vita2d_texture_get_stride(texture)
#define GUI_GetTextureFormat(texture) vita2d_texture_get_format(texture)
#define GUI_GetTextureDatap(texture) vita2d_texture_get_datap(texture)
#define GUI_GetTexturePalette(texture) vita2d_texture_get_palette(texture)
#define GUI_GetTextureMinFilter(texture) vita2d_texture_get_min_filter(texture)
#define GUI_GetTextureMagFilter(texture) vita2d_texture_get_mag_filter(texture)
#define GUI_SetTextureFilter(texture, min_filter, mag_filter) vita2d_texture_set_filters(texture, min_filter, mag_filter)

#define GUI_DrawTexture(texture, x, y) vita2d_draw_texture(texture, x, y)
#define GUI_DrawTextureRotate(texture, x, y, rad) vita2d_draw_texture_rotate(texture, x, y, rad)
#define GUI_DrawTextureScale(texture, x, y, x_scale, y_scale) vita2d_draw_texture_scale(texture, x, y, x_scale, y_scale)
#define GUI_DrawTexturePart(texture, x, y, tex_x, tex_y, tex_w, tex_h) vita2d_draw_texture_part(texture, x, y, tex_x, tex_y, tex_w, tex_h)
#define GUI_DrawTexturePartScale(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale) \
    vita2d_draw_texture_part_scale(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale)
#define GUI_DrawTexturePartScaleRotate(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, rad) \
    vita2d_draw_texture_part_scale_rotate(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, rad)

#define GUI_DrawTextureTint(texture, x, y, color) vita2d_draw_texture_tint(texture, x, y)
#define GUI_DrawTextureTintRotate(texture, x, y, rad, color) vita2d_draw_texture_tint_rotate(texture, x, y, rad)
#define GUI_DrawTextureTintScale(texture, x, y, x_scale, y_scale, color) vita2d_draw_texture_tint_scale(texture, x, y, x_scale, y_scale)
#define GUI_DrawTextureTintPart(texture, x, y, tex_x, tex_y, tex_w, tex_h, color) vita2d_draw_texture_tint_part(texture, x, y, tex_x, tex_y, tex_w, tex_h)
#define GUI_DrawTextureTintPartScale(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, color) \
    vita2d_draw_texture_tint_part_scale(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, color)
#define GUI_DrawTextureTintPartScaleRotate(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, rad, color) \
    vita2d_draw_texture_part_tint_scale_rotate(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, rad, color)

void GUI_DrawTextureShaderPartScalRotate(const GUI_Texture *texture, const GUI_Shader *shader, float x, float y,
                                         float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale, float rad);

#endif
