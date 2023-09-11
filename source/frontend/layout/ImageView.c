#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"
#include "ImageView.h"

void ImageViewDestroy(void *view)
{
    ImageView *imageView = (ImageView *)view;

    if (!imageView || imageView->data.params.donot_free)
        return;

    if (imageView->tex)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(imageView->tex);
    }
    free(imageView);
}

int ImageViewUpdate(void *view, int x, int y, int max_w, int max_h)
{
    if (!view)
        return -1;

    ImageView *imageView = (ImageView *)view;
    LayoutPosition *positions = &imageView->data.positions;

    positions->layout_max_w = max_w;
    positions->layout_max_h = max_h;
    positions->x = x;
    positions->y = y;
    positions->w = positions->layout_w;
    positions->h = positions->layout_h;

    if (positions->layout_w == TYPE_LAYOUT_MATH_PARENT)
        positions->w = max_w;
    else if (positions->layout_w == TYPE_LAYOUT_WRAP_CONTENT)
        positions->w = imageView->tex_w;
    if (positions->w > max_w)
        positions->w = max_w;

    if (positions->layout_h == TYPE_LAYOUT_MATH_PARENT)
        positions->h = max_h;
    else if (positions->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
        positions->h = imageView->tex_h;
    if (positions->h > max_h)
        positions->h = max_h;

    if (imageView->tex)
    {
        int max_w = positions->w - positions->padding_left - positions->padding_right;
        int max_h = positions->w - positions->padding_top - positions->padding_bottom;
        int tex_w = imageView->tex_w;
        int tex_h = imageView->tex_h;
        int scale_w = tex_w;
        int scale_h = tex_h;
        int scale_src_x = 0;
        int scale_src_y = 0;
        int scale_src_w = scale_w;
        int scale_src_h = scale_h;
        float aspect_ratio = (float)tex_w / (float)tex_h;

        switch (imageView->scale_type)
        {
        case TYPE_IMAGE_SCALE_CENTER:
            if (tex_w > max_w || tex_h > max_h)
                goto SCALE_CENTER_CROP;
        SMALL_CENTER:
            scale_w = tex_w;
            scale_h = tex_h;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = (max_w - scale_w) / 2;
            imageView->tex_dst_y = (max_h - scale_h) / 2;
            break;

        case TYPE_IMAGE_SCALE_CENTER_CROP:
        SCALE_CENTER_CROP:
            scale_w = tex_w;
            scale_h = tex_h;
            if (scale_w < max_w)
            {
                scale_w = max_w;
                scale_h = scale_w / aspect_ratio;
            }
            if (scale_h < max_h)
            {
                scale_h = max_h;
                scale_w = scale_h * aspect_ratio;
            }

            if (scale_w > max_w)
            {
                scale_src_x = (scale_w - max_w) / 2;
                imageView->tex_dst_x = 0;
            }
            else
            {
                scale_src_x = 0;
                imageView->tex_dst_x = (max_w - scale_w) / 2;
            }
            if (scale_h > max_h)
            {
                scale_src_y = (scale_h - max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            else
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (max_h - scale_h) / 2;
            }
            break;

        case TYPE_IMAGE_SCALE_CENTER_INSIDE:
            if (tex_w > max_w || tex_h > max_h)
                goto SCALE_FIT_CENTER;
            goto SMALL_CENTER;
            break;

        case TYPE_IMAGE_SCALE_FIT_XY:
            scale_w = max_w;
            scale_h = max_h;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = 0;
            imageView->tex_dst_y = 0;
            break;

        case TYPE_IMAGE_SCALE_FIT_START:
            scale_w = max_w;
            scale_h = scale_w / aspect_ratio;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = 0;
            imageView->tex_dst_y = 0;
            break;

        case TYPE_IMAGE_SCALE_FIT_END:
            scale_w = max_w;
            scale_h = scale_w / aspect_ratio;
            scale_src_x = 0;
            imageView->tex_dst_x = 0;
            if (scale_h > max_h)
            {
                scale_src_y = scale_h - max_h;
                imageView->tex_dst_y = 0;
            }
            else
            {
                scale_src_y = 0;
                imageView->tex_dst_y = max_h - scale_h;
            }
            break;

        case TYPE_IMAGE_SCALE_FIT_CENTER:
        SCALE_FIT_CENTER:
            scale_w = max_w;
            scale_h = scale_w / aspect_ratio;
            if (scale_h > max_h)
            {
                scale_h = max_h;
                scale_w = scale_h * aspect_ratio;
            }
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = (max_w - scale_w) / 2;
            imageView->tex_dst_y = (max_h - scale_h) / 2;
            break;

        case TYPE_IMAGE_SCALE_MATRIX:
            scale_w = tex_w;
            scale_h = tex_h;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = 0;
            imageView->tex_dst_y = 0;
            break;

        default:
            break;
        }

        scale_src_w = scale_w > max_w ? max_w : scale_w;
        scale_src_h = scale_h > max_h ? max_h : scale_h;

        imageView->tex_scale_x = (float)scale_w / (float)tex_w;
        imageView->tex_scale_y = (float)scale_h / (float)tex_h;
        imageView->tex_src_x = scale_src_x / imageView->tex_scale_x;
        imageView->tex_src_y = scale_src_y / imageView->tex_scale_y;
        imageView->tex_src_w = scale_src_w / imageView->tex_scale_x;
        imageView->tex_src_h = scale_src_h / imageView->tex_scale_y;
    }

    return 0;
}

void ImageViewDraw(void *view)
{
    if (!view)
        return;

    ImageView *imageView = (ImageView *)view;
    LayoutPosition *positions = &imageView->data.positions;

    if (imageView->bg_color)
        GUI_DrawFillRectangle(positions->x, positions->y, positions->w, positions->h, imageView->bg_color);

    if (imageView->tex)
    {
        int tex_dst_x = positions->x + positions->padding_left + imageView->tex_dst_x;
        int tex_dst_y = positions->y + positions->padding_top + imageView->tex_dst_y;

        if (imageView->tint_color)
            GUI_DrawTextureTintPartScale(imageView->tex, tex_dst_x, tex_dst_y, imageView->tex_src_x, imageView->tex_src_y,
                                         imageView->tex_src_w, imageView->tex_src_h, imageView->tex_scale_x, imageView->tex_scale_y, imageView->tint_color);
        else
            GUI_DrawTexturePartScale(imageView->tex, tex_dst_x, tex_dst_y, imageView->tex_src_x, imageView->tex_src_y,
                                     imageView->tex_src_w, imageView->tex_src_h, imageView->tex_scale_x, imageView->tex_scale_y);
    }
}

int ImageViewSetScaleType(ImageView *view, int type)
{
    if (!view)
        return -1;

    view->scale_type = type;

    return 0;
}

int ImageViewSetPadding(ImageView *view, int left, int right, int top, int bottom)
{
    if (!view)
        return -1;

    LayoutPosition *positions = &view->data.positions;

    positions->padding_left = left;
    positions->padding_right = right;
    positions->padding_top = top;
    positions->padding_bottom = bottom;

    return 0;
}

int ImageViewSetMargin(ImageView *view, int left, int right, int top, int bottom)
{
    if (!view)
        return -1;

    LayoutPosition *positions = &view->data.positions;

    positions->margin_left = left;
    positions->margin_right = right;
    positions->margin_top = top;
    positions->margin_bottom = bottom;

    return 0;
}

int ImageViewSetTintColor(ImageView *view, uint32_t color)
{
    if (!view)
        return -1;

    view->tint_color = color;

    return 0;
}

int ImageViewSetBgColor(ImageView *view, uint32_t color)
{
    if (!view)
        return -1;

    view->bg_color = color;

    return 0;
}

GUI_Texture *ImageViewGetTexture(ImageView *view)
{
    return view ? view->tex : NULL;
}

int ImageViewSetTexture(ImageView *view, GUI_Texture *tex)
{
    if (!view)
        return -1;

    if (view->tex)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(view->tex);
    }
    if (tex)
    {
        view->tex = tex;
        view->tex_w = GUI_GetTextureWidth(view->tex);
        view->tex_h = GUI_GetTextureHeight(view->tex);
    }

    return 0;
}

int ImageViewRefresh(ImageView *view)
{
    if (!view)
        return -1;

    LayoutPosition *positions = &view->data.positions;

    if (positions->layout_w > 0 && positions->layout_h > 0)
        return ImageViewUpdate(view, positions->x, positions->y, positions->layout_w, positions->layout_h);
    else if (view->data.params.parent)
        return LayoutRefresh(view->data.params.parent);
    else
        return ImageViewUpdate(view, positions->x, positions->y, positions->layout_max_w, positions->layout_max_h);
}

ImageView *ImageViewCreat(int layout_w, int layout_h, int donot_free)
{
    ImageView *view = (ImageView *)calloc(1, sizeof(ImageView));
    if (!view)
        return NULL;

    LayoutParam *params = &view->data.params;
    LayoutPosition *positions = &view->data.positions;

    positions->layout_w = layout_w;
    positions->layout_h = layout_h;
    params->donot_free = donot_free;
    params->update = ImageViewUpdate;
    params->draw = ImageViewDraw;
    params->destroy = ImageViewDestroy;

    return view;
}

ImageView *ImageViewCreatFrom(const char *path, int layout_w, int layout_h, int donot_free)
{
    ImageView *view = ImageViewCreat(layout_w, layout_h, donot_free);
    if (!view)
        return NULL;

    view->tex = GUI_LoadPNGFile(path);

    return view;
}
