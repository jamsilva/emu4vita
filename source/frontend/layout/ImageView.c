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

    if (!imageView || imageView->params.dont_free)
        return;

    if (imageView->tex && imageView->tex_auto_free)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(imageView->tex);
    }
    free(imageView);
}

int ImageViewUpdate(void *view, int remaining_w, int remaining_h)
{
    if (!view)
        return -1;

    ImageView *imageView = (ImageView *)view;
    LayoutParam *params = &imageView->params;

    int max_w = remaining_w - params->margin_left - params->margin_right;
    int max_h = remaining_h - params->margin_top - params->margin_bottom;
    int wrap_w = imageView->tex_w + params->padding_left + params->padding_right;
    int wrap_h = imageView->tex_h + params->padding_top + params->padding_bottom;

    params->measured_w = params->layout_w;
    params->measured_h = params->layout_h;

    if (params->layout_w == TYPE_LAYOUT_MATH_PARENT)
        params->measured_w = max_w;
    else if (params->layout_w == TYPE_LAYOUT_WRAP_CONTENT)
        params->measured_w = wrap_w;
    if (params->measured_w > max_w)
        params->measured_w = max_w;
    if (params->measured_w < 0)
        params->measured_w = 0;

    if (params->layout_h == TYPE_LAYOUT_MATH_PARENT)
        params->measured_h = max_h;
    else if (params->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
        params->measured_h = wrap_h;
    if (params->measured_h > max_h)
        params->measured_h = max_h;
    if (params->measured_h < 0)
        params->measured_h = 0;

    if (imageView->tex)
    {
        int tex_max_w = params->measured_w - params->padding_left - params->padding_right;
        int tex_max_h = params->measured_h - params->padding_top - params->padding_bottom;
        int tex_w = imageView->tex_w;
        int tex_h = imageView->tex_h;
        int scale_w = tex_w;
        int scale_h = tex_h;
        int scale_src_x = 0;
        int scale_src_y = 0;
        int scale_src_w = tex_w;
        int scale_src_h = tex_h;
        float aspect_ratio = (float)tex_w / (float)tex_h;

        switch (imageView->scale_type)
        {
        case TYPE_IMAGE_SCALE_CENTER: // 原图，居中，（宽高度大于视图：裁剪）
                                      //            （宽高度小于视图：原图）
            scale_w = tex_w;
            scale_h = tex_h;
            if (scale_w < tex_max_w)
            {
                scale_src_x = 0;
                imageView->tex_dst_x = (tex_max_w - scale_w) / 2;
            }
            else
            {
                scale_src_x = (scale_w - tex_max_w) / 2;
                imageView->tex_dst_x = 0;
            }
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            }
            else
            {
                scale_src_y = (scale_h - tex_max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_CENTER_CROP: // 等比，居中，（宽高度小于视图：先放大，再裁剪）
                                           //            （宽高度大于视图：原图）
            scale_w = tex_w;
            scale_h = tex_h;
            if (scale_w < tex_max_w) // 宽度小于视图，放大
            {
                scale_w = tex_max_w;
                scale_h = scale_w / aspect_ratio;
            }
            if (scale_h < tex_max_h) // 高度小于视图，放大
            {
                scale_h = tex_max_h;
                scale_w = scale_h * aspect_ratio;
            }

            if (scale_w < tex_max_w)
            {
                scale_src_x = 0;
                imageView->tex_dst_x = (tex_max_w - scale_w) / 2;
            }
            else
            {
                scale_src_x = (scale_w - tex_max_w) / 2;
                imageView->tex_dst_x = 0;
            }
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            }
            else
            {
                scale_src_y = (scale_h - tex_max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_CENTER_INSIDE: // 等比，居中，（宽高度大于视图： 缩小 （宽==视图，高<=视图） 或 （高===视图，宽<=视图））
                                             //             (宽高度小于视图： 原图
            scale_w = tex_w;
            scale_h = tex_h;
            if (scale_w > tex_max_w) // 宽度大于视图，缩小
            {
                scale_w = tex_max_w;
                scale_h = scale_w / aspect_ratio;
            }
            if (scale_h > tex_max_h) // 高度大于视图，缩小
            {
                scale_h = tex_max_h;
                scale_w = scale_h * aspect_ratio;
            }

            if (scale_w < tex_max_w)
            {
                scale_src_x = 0;
                imageView->tex_dst_x = (tex_max_w - scale_w) / 2;
            }
            else
            {
                scale_src_x = (scale_w - tex_max_w) / 2;
                imageView->tex_dst_x = 0;
            }
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            }
            else
            {
                scale_src_y = (scale_h - tex_max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_FIT_XY: // 拉伸，缩放，（宽==视图，高==视图）
            scale_w = tex_max_w;
            scale_h = tex_max_h;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = 0;
            imageView->tex_dst_y = 0;
            break;

        case TYPE_IMAGE_SCALE_FIT_START: // 等比，缩放，顶部对齐，（宽==视图，高等比缩放）
            scale_w = tex_max_w;
            scale_h = scale_w / aspect_ratio;
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = 0;
            imageView->tex_dst_y = 0;
            break;

        case TYPE_IMAGE_SCALE_FIT_END: // 等比，缩放，底部对齐，（宽==视图，高等比缩放）
            scale_w = tex_max_w;
            scale_h = scale_w / aspect_ratio;
            scale_src_x = 0;
            imageView->tex_dst_x = 0;
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = tex_max_h - scale_h;
            }
            else
            {
                scale_src_y = scale_h - tex_max_h;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_FIT_CENTER: // 等比，缩放，垂直居中，（宽==视图，高等比缩放）
            scale_w = tex_max_w;
            scale_h = scale_w / aspect_ratio;
            scale_src_x = 0;
            imageView->tex_dst_x = 0;
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            }
            else
            {
                scale_src_y = (scale_h - tex_max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_FIT_CENTER_CROP: // 等比，居中，（宽高度大于视图： 缩小 （宽==视图，高>=视图） 或 （高===视图，宽>=视图））
                                               //             (宽高度小于视图： 放大 （宽==视图，高>=视图） 或 （高===视图，宽>=视图））
            scale_w = tex_max_w;
            scale_h = scale_w / aspect_ratio;
            if (scale_h < tex_max_h)
            {
                scale_h = tex_max_h;
                scale_w = scale_h * aspect_ratio;
            }

            if (scale_w < tex_max_w)
            {
                scale_src_x = 0;
                imageView->tex_dst_x = (tex_max_w - scale_w) / 2;
            }
            else
            {
                scale_src_x = (scale_w - tex_max_w) / 2;
                imageView->tex_dst_x = 0;
            }
            if (scale_h < tex_max_h)
            {
                scale_src_y = 0;
                imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            }
            else
            {
                scale_src_y = (scale_h - tex_max_h) / 2;
                imageView->tex_dst_y = 0;
            }
            break;

        case TYPE_IMAGE_SCALE_FIT_CENTER_INSIDE: // 等比，居中，（宽高度大于视图： 缩小 （宽==视图，高<=视图） 或 （高===视图，宽<=视图））
                                                 //             (宽高度小于视图： 放大 （宽==视图，高<=视图） 或 （高===视图，宽<=视图））
            scale_w = tex_max_w;
            scale_h = scale_w / aspect_ratio;
            if (scale_h > tex_max_h)
            {
                scale_h = tex_max_h;
                scale_w = scale_h * aspect_ratio;
            }
            scale_src_x = 0;
            scale_src_y = 0;
            imageView->tex_dst_x = (tex_max_w - scale_w) / 2;
            imageView->tex_dst_y = (tex_max_h - scale_h) / 2;
            break;

        case TYPE_IMAGE_SCALE_MATRIX: // 图像原始大小，左上对齐
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

        scale_src_w = scale_w < tex_max_w ? scale_w : tex_max_w;
        scale_src_h = scale_h < tex_max_h ? scale_h : tex_max_h;

        imageView->tex_scale_x = (float)scale_w / (float)tex_w;
        imageView->tex_scale_y = (float)scale_h / (float)tex_h;
        imageView->tex_src_x = scale_src_x / imageView->tex_scale_x;
        imageView->tex_src_y = scale_src_y / imageView->tex_scale_y;
        imageView->tex_src_w = scale_src_w / imageView->tex_scale_x;
        imageView->tex_src_h = scale_src_h / imageView->tex_scale_y;
    }

    return 0;
}

void ImageViewDraw(void *view, int x, int y)
{
    if (!view)
        return;

    ImageView *imageView = (ImageView *)view;
    LayoutParam *params = &imageView->params;

    if (params->measured_w <= 0 || params->measured_h <= 0)
        return;

    int view_x = x + params->margin_left;
    int view_y = y + params->margin_top;

    if (imageView->bg_color)
        GUI_DrawFillRectangle(view_x, view_y, params->measured_w, params->measured_h, imageView->bg_color);

    if (imageView->tex)
    {
        int tex_dst_x = view_x + params->padding_left + imageView->tex_dst_x;
        int tex_dst_y = view_y + params->padding_top + imageView->tex_dst_y;

        if (imageView->tint_color)
            GUI_DrawTextureTintPartScale(imageView->tex, tex_dst_x, tex_dst_y, imageView->tex_src_x, imageView->tex_src_y,
                                         imageView->tex_src_w, imageView->tex_src_h, imageView->tex_scale_x, imageView->tex_scale_y, imageView->tint_color);
        else
            GUI_DrawTexturePartScale(imageView->tex, tex_dst_x, tex_dst_y, imageView->tex_src_x, imageView->tex_src_y,
                                     imageView->tex_src_w, imageView->tex_src_h, imageView->tex_scale_x, imageView->tex_scale_y);
    }
}

int ImageViewSetScaleType(ImageView *imageView, int type)
{
    if (!imageView)
        return -1;

    imageView->scale_type = type;

    return 0;
}

int ImageViewSetTintColor(ImageView *imageView, uint32_t color)
{
    if (!imageView)
        return -1;

    imageView->tint_color = color;

    return 0;
}

int ImageViewSetBgColor(ImageView *imageView, uint32_t color)
{
    if (!imageView)
        return -1;

    imageView->bg_color = color;

    return 0;
}

GUI_Texture *ImageViewGetTexture(ImageView *imageView)
{
    return imageView ? imageView->tex : NULL;
}

int ImageViewSetTexture(ImageView *imageView, GUI_Texture *tex, int auto_free)
{
    if (!imageView)
        return -1;

    if (imageView->tex && imageView->tex_auto_free)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(imageView->tex);
    }

    imageView->tex = tex;
    imageView->tex_auto_free = auto_free;
    imageView->tex_w = 0;
    imageView->tex_h = 0;

    if (imageView->tex)
    {
        imageView->tex_w = GUI_GetTextureWidth(imageView->tex);
        imageView->tex_h = GUI_GetTextureHeight(imageView->tex);
    }

    return 0;
}

int ImageViewInit(ImageView *imageView)
{
    if (!imageView)
        return -1;

    memset(imageView, 0, sizeof(ImageView));

    LayoutParam *params = &imageView->params;
    params->destroy = ImageViewDestroy;
    params->update = ImageViewUpdate;
    params->draw = ImageViewDraw;

    return 0;
}

ImageView *NewImageView()
{
    ImageView *imageView = (ImageView *)malloc(sizeof(ImageView));
    if (!imageView)
        return NULL;

    ImageViewInit(imageView);

    return imageView;
}

ImageView *NewImageViewFrom(const char *path)
{
    ImageView *imageView = NewImageView();
    if (!imageView)
        return NULL;

    GUI_Texture *tex = GUI_LoadPNGFile(path);
    ImageViewSetTexture(imageView, tex, 1);

    return imageView;
}
