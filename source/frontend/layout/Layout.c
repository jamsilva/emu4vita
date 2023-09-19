#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "list/linked_list.h"
#include "gui/gui.h"
#include "Layout.h"

void LayoutDestroy(void *view)
{
    Layout *layout = (Layout *)view;

    if (!layout || layout->params.dont_free)
        return;

    LayoutEmpty(layout);
    LinkedListDestroy(layout->childs);
    free(layout);
}

static int LayoutUpdateChild(void *view, int orientation, int *remaining_w, int *remaining_h)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;

    if (!params->update)
        return -1;

    params->update(params, *remaining_w, *remaining_h);

    if (orientation == TYPE_LAYOUT_ORIENTATION_HORIZONTAL)
    {
        *remaining_w -= (params->measured_w + params->margin_left + params->margin_right);
        if (params->above_view)
            LayoutUpdateChild(params->above_view, orientation, remaining_w, remaining_h);
    }
    else if (orientation == TYPE_LAYOUT_ORIENTATION_VERTICAL)
    {
        *remaining_h -= (params->measured_h + params->margin_top + params->margin_bottom);
        if (params->above_view)
            LayoutUpdateChild(params->above_view, orientation, remaining_w, remaining_h);
    }

    return 0;
}

int LayoutUpdate(void *view, int remaining_w, int remaining_h)
{
    if (!view)
        return -1;

    Layout *layout = (Layout *)view;
    LayoutParam *params = &layout->params;
    int max_w = remaining_w - params->margin_left - params->margin_right;
    int max_h = remaining_h - params->margin_top - params->margin_bottom;
    int wrap_w = 0;
    int wrap_h = 0;

    layout->remaining_w = max_w - params->padding_left - params->padding_right;
    layout->remaining_h = max_h - params->padding_top - params->padding_bottom;

    LinkedListEntry *entry = LinkedListHead(layout->childs);

    while (entry)
    {
        if (layout->remaining_w <= 0 || layout->remaining_h <= 0)
            break;

        void *child = LinkedListGetEntryData(entry);
        LayoutUpdateChild(child, layout->orientation, &layout->remaining_w, &layout->remaining_h);

        if (max_w - layout->remaining_w > wrap_w)
            wrap_w = max_w - layout->remaining_w;
        if (max_h - layout->remaining_h > wrap_h)
            wrap_h = max_h - layout->remaining_h;

        entry = LinkedListNext(entry);
    }

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

static void LayoutDrawChild(void *view, int orientation, int *x, int *y)
{
    if (!view)
        return;

    LayoutParam *params = (LayoutParam *)view;

    if (params->above_view)
        LayoutDrawChild(params->above_view, orientation, x, y);

    if (!params->draw)
        return;

    params->draw(params, *x, *y);

    if (orientation == TYPE_LAYOUT_ORIENTATION_HORIZONTAL)
        *x += (params->measured_w + params->margin_left + params->margin_right);
    else if (orientation == TYPE_LAYOUT_ORIENTATION_VERTICAL)
        *y += (params->measured_h + params->margin_top + params->margin_bottom);
}

void LayoutDraw(void *view, int x, int y)
{
    if (!view)
        return;

    Layout *layout = (Layout *)view;
    LayoutParam *params = &layout->params;

    if (params->measured_w <= 0 || params->measured_h <= 0)
        return;

    int layout_x = x + params->margin_left;
    int layout_y = y + params->margin_top;

    if (layout->bg_color)
        GUI_DrawFillRectangle(layout_x, layout_y, params->measured_w, params->measured_h, layout->bg_color);

    LinkedListEntry *entry = LinkedListHead(layout->childs);
    int child_min_x = layout_x + params->padding_left;
    int child_min_y = layout_y + params->padding_top;
    int child_max_x = layout_x + params->measured_w - params->padding_right;
    int child_max_y = layout_y + params->measured_h - params->padding_bottom;
    int child_max_w = child_max_x - child_min_x;
    int child_max_h = child_max_y - child_min_y;

    GUI_EnableClipping(child_min_x, child_min_y, child_max_w, child_max_h);

    int child_x = child_min_x;
    int child_y = child_min_y;

    while (entry)
    {
        if (child_x >= child_max_x || child_y >= child_max_y)
            break;

        void *child = LinkedListGetEntryData(entry);
        LayoutDrawChild(child, layout->orientation, &child_x, &child_y);

        entry = LinkedListNext(entry);
    }

    GUI_DisableClipping();
}

int LayoutSetOrientation(Layout *layout, int orientation)
{
    if (!layout)
        return -1;

    layout->orientation = orientation;

    return 0;
}

int LayoutSetBgColor(Layout *layout, uint32_t color)
{
    if (!layout)
        return -1;

    layout->bg_color = color;

    return 0;
}

int LayoutGetRemaining(Layout *layout, int *remaining_w, int *remaining_h)
{
    if (!layout)
        return -1;

    *remaining_w = layout->remaining_w;
    *remaining_h = layout->remaining_h;

    return 0;
}

int LayoutAdd(Layout *layout, void *view)
{
    if (!layout || !layout->childs || !view)
        return 0;

    LayoutParam *v_params = (LayoutParam *)view;
    v_params->parent = layout;
    LinkedListAddEx(layout->childs, view, v_params->destroy);

    return 1;
}

int LayoutRemove(Layout *layout, void *view)
{
    if (!layout || !layout->childs || !view)
        return 0;

    LayoutParam *v_params = (LayoutParam *)view;
    if (v_params->parent != layout)
        return 0;

    LinkedListEntry *entry = LinkedListFindByData(layout->childs, view);
    if (!entry)
        return 0;

    if (v_params->above_view)
        ViewDestroy(v_params->above_view);
    LinkedListRemove(layout->childs, entry);

    return 1;
}

void LayoutEmpty(Layout *layout)
{
    if (!layout)
        return;

    LinkedListEntry *entry = LinkedListHead(layout->childs);
    while (entry)
    {
        LinkedListEntry *next = LinkedListNext(entry);
        LayoutParam *params = (LayoutParam *)LinkedListGetEntryData(entry);
        if (params->above_view)
            ViewDestroy(params->above_view);
        LinkedListRemove(layout->childs, entry);
        entry = next;
    }
}

int LayoutInit(Layout *layout)
{
    if (!layout)
        return -1;

    memset(layout, 0, sizeof(Layout));

    layout->childs = NewLinkedList();
    if (!layout->childs)
        return -1;

    LayoutParam *params = &layout->params;
    params->destroy = LayoutDestroy;
    params->update = LayoutUpdate;
    params->draw = LayoutDraw;

    return 0;
}

Layout *NewLayout()
{
    Layout *layout = (Layout *)malloc(sizeof(Layout));
    if (!layout)
        return NULL;

    if (LayoutInit(layout) < 0)
    {
        free(layout);
        return NULL;
    }

    return layout;
}

int LayoutParamSetMargin(void *view, int left, int right, int top, int bottom)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    params->margin_left = left;
    params->margin_right = right;
    params->margin_top = top;
    params->margin_bottom = bottom;

    return 0;
}

int LayoutParamSetPadding(void *view, int left, int right, int top, int bottom)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    params->padding_left = left;
    params->padding_right = right;
    params->padding_top = top;
    params->padding_bottom = bottom;

    return 0;
}

int LayoutParamSetLayoutSize(void *view, int layout_w, int layout_h)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    params->layout_w = layout_w;
    params->layout_h = layout_h;

    return 0;
}

int LayoutParamSetAutoFree(void *view, int auto_free)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    params->dont_free = !auto_free;

    return 0;
}

int RootViewUpdate(void *view)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    if (!params->update)
        return -1;

    int max_w, max_h;

    if (params->layout_w > 0)
        max_w = params->layout_w;
    else
        max_w = GUI_SCREEN_WIDTH;
    if (params->layout_h > 0)
        max_h = params->layout_h;
    else
        max_h = GUI_SCREEN_HEIGHT;

    return params->update(view, max_w, max_h);
}

int ViewRefresh(void *view)
{
    if (!view)
        return -1;

    LayoutParam *params = (LayoutParam *)view;
    if (!params->update)
        return -1;

    if (params->layout_w != TYPE_LAYOUT_WRAP_CONTENT && params->layout_h != TYPE_LAYOUT_WRAP_CONTENT &&
        params->measured_w > 0 && params->measured_h > 0)
        return params->update(view, params->measured_w + params->margin_left + params->margin_right, params->measured_h + params->margin_top + params->margin_bottom);
    else if (params->parent)
        return ViewRefresh(params->parent);
    else
        return RootViewUpdate(view);

    return 0;
}

void ViewDestroy(void *view)
{
    if (!view)
        return;

    LayoutParam *params = (LayoutParam *)view;
    if (params->above_view)
        ViewDestroy(params->above_view);
    if (params->destroy)
        params->destroy(params);
}

int ViewAddAbove(void *view, void *above)
{
    if (!view)
        return 0;

    LayoutParam *params = (LayoutParam *)view;
    params->above_view = above;

    return 1;
}
