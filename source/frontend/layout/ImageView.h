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
    TYPE_IMAGE_SCALE_FIT_CENTER_CROP,
    TYPE_IMAGE_SCALE_FIT_CENTER_INSIDE,
    TYPE_IMAGE_SCALE_MATRIX,
};

typedef struct
{
    LayoutParam params;
    GUI_Texture *tex;
    int tex_auto_free;
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
int ImageViewUpdate(void *view, int remaining_w, int remaining_h);
void ImageViewDraw(void *view, int x, int y);

int ImageViewSetScaleType(ImageView *imageView, int type);
int ImageViewSetTintColor(ImageView *imageView, uint32_t color);
int ImageViewSetBgColor(ImageView *imageView, uint32_t color);
int ImageViewSetTexture(ImageView *imageView, GUI_Texture *tex, int auto_free);
GUI_Texture *ImageViewGetTexture(ImageView *imageView);

int ImageViewInit(ImageView *imageView);
ImageView *NewImageView();
ImageView *NewImageViewFrom(const char *path);

#endif
