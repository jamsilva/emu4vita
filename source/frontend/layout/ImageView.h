#ifndef __M_IMAGE_VIEW_H__
#define __M_IMAGE_VIEW_H__

#include "gui/gui.h"
#include "Layout.h"

enum TypeImageViewScale
{
    TYPE_IMAGE_SCALE_CENTER,
    TYPE_IMAGE_SCALE_CENTER_CROP,
    TYPE_IMAGE_SCALE_CENTER_INSIDE,
    TYPE_IMAGE_SCALE_FIT_XY,
    TYPE_IMAGE_SCALE_FIT_START,
    TYPE_IMAGE_SCALE_FIT_END,
    TYPE_IMAGE_SCALE_FIT_CENTER,
    TYPE_IMAGE_SCALE_MATRIX,
};

typedef struct ImageView
{
    ViewData data;
    GUI_Texture *tex;
    int scale_type;
    int tex_w;
    int tex_h;
    int tex_dst_x;
    int tex_dst_y;
    int tex_src_x;
    int tex_src_y;
    int tex_src_w;
    int tex_src_h;
    float tex_scale_x;
    float tex_scale_y;
    uint32_t tint_color;
    uint32_t bg_color;
} ImageView;

void ImageViewDestroy(void *view);
int ImageViewUpdate(void *view, int x, int y, int max_w, int max_h);
void ImageViewDraw(void *view);

int ImageViewSetScaleType(ImageView *view, int type);
int ImageViewSetPadding(ImageView *view, int left, int right, int top, int bottom);
int ImageViewSetMargin(ImageView *view, int left, int right, int top, int bottom);
int ImageViewSetTintColor(ImageView *view, uint32_t color);
int ImageViewSetBgColor(ImageView *view, uint32_t color);
GUI_Texture *ImageViewGetTexture(ImageView *view);
int ImageViewSetTexture(ImageView *view, GUI_Texture *tex);

int ImageViewRefresh(ImageView *view);
ImageView *ImageViewCreat(int layout_w, int layout_h, int donot_free);
ImageView *ImageViewCreatFrom(const char *path, int layout_w, int layout_h, int donot_free);

#endif
