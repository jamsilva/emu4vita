#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"
#include "ItemView.h"

void ItemViewDestroy(void *view)
{
    ItemView *itemView = (ItemView *)view;

    if (!itemView || itemView->params.dont_free)
        return;

    if (itemView->name)
        free(itemView->name);
    if (itemView->info)
        free(itemView->info);
    free(itemView);
}

int ItemViewUpdate(void *view, int remaining_w, int remaining_h)
{
    if (!view)
        return -1;

    ItemView *itemView = (ItemView *)view;
    LayoutParam *params = &itemView->params;

    int max_w = remaining_w - params->margin_left - params->margin_right;
    int max_h = remaining_h - params->margin_top - params->margin_bottom;
    int wrap_w = 0;
    int wrap_h = 0;

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

    return 0;
}

void ItemViewDraw(void *view, int x, int y)
{
    if (!view)
        return;

    ItemView *itemView = (ItemView *)view;
    LayoutParam *params = &itemView->params;

    if (params->measured_w <= 0 || params->measured_h <= 0)
        return;

    int view_x = x + params->margin_left;
    int view_y = y + params->margin_top;

    if (itemView->bg_color)
        GUI_DrawFillRectangle(view_x, view_y, params->measured_w, params->measured_h, itemView->bg_color);
}

int ItemViewInit(ItemView *itemView)
{
    if (!itemView)
        return -1;

    memset(itemView, 0, sizeof(ItemView));

    LayoutParam *params = &itemView->params;
    params->destroy = ItemViewDestroy;
    params->update = ItemViewUpdate;
    params->draw = ItemViewDraw;

    return 0;
}

ItemView *NewItemView()
{
    ItemView *itemView = (ItemView *)malloc(sizeof(ItemView));
    if (!itemView)
        return NULL;

    ItemViewInit(itemView);

    return itemView;
}
