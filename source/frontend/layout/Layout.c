#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"

void LayoutDestroy(void *view)
{
    Layout *layout = (Layout *)view;

    if (!layout || layout->data.params.donot_free)
        return;

    if (layout->childs)
    {
        LinkedListEntry *entry = LinkedListHead(layout->childs);
        while (entry)
        {
            LinkedListEntry *next = LinkedListNext(entry);
            ViewData *data = (ViewData *)LinkedListGetEntryData(entry);
            LayoutParam *params = &data->params;
            if (params->destroy)
                params->destroy(data);
            LinkedListRemove(layout->childs, entry);
            entry = next;
        }
    }

    LinkedListDestroy(layout->childs);
    free(layout);
}

int LayoutUpdate(void *view, int x, int y, int max_w, int max_h)
{
    if (!view)
        return -1;

    Layout *layout = (Layout *)view;
    LayoutPosition *ly_positions = &layout->data.positions;

    ly_positions->layout_max_w = max_w;
    ly_positions->layout_max_h = max_h;

    if (ly_positions->layout_w > 0 && ly_positions->layout_w < max_w)
        max_w = ly_positions->layout_w;
    if (ly_positions->layout_h > 0 && ly_positions->layout_h < max_w)
        max_h = ly_positions->layout_h;

    layout->remaining_x = x;
    layout->remaining_y = x;
    layout->remaining_w = max_w;
    layout->remaining_h = max_h;

    LinkedListEntry *entry = LinkedListHead(layout->childs);

    while (entry)
    {
        ViewData *data = (ViewData *)LinkedListGetEntryData(entry);
        LayoutParam *params = &data->params;
        LayoutPosition *positions = &data->positions;

        if (params->update)
            params->update(data, layout->remaining_x, layout->remaining_y, layout->remaining_w, layout->remaining_h);

        if (layout->orientation == TYPE_LAYOUT_ORIENTATION_HORIZONTAL)
        {
            layout->remaining_x += positions->w;
            layout->remaining_w -= positions->w;
            if (layout->remaining_w <= 0)
                break;
        }
        else if (layout->orientation == TYPE_LAYOUT_ORIENTATION_VERTICAL)
        {
            layout->remaining_y += positions->w;
            layout->remaining_h -= positions->h;
            if (layout->remaining_h <= 0)
                break;
        }
    }

    ly_positions->x = x;
    ly_positions->y = y;
    ly_positions->w = ly_positions->layout_w;
    ly_positions->h = ly_positions->layout_h;

    if (ly_positions->layout_w == TYPE_LAYOUT_MATH_PARENT)
        ly_positions->w = max_w;
    else if (ly_positions->layout_w == TYPE_LAYOUT_WRAP_CONTENT)
        ly_positions->w = layout->remaining_x - x;
    if (ly_positions->w > max_w)
        ly_positions->w = max_w;

    if (ly_positions->layout_h == TYPE_LAYOUT_MATH_PARENT)
        ly_positions->h = max_h;
    else if (ly_positions->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
        ly_positions->h = layout->remaining_y - y;
    if (ly_positions->h > max_h)
        ly_positions->h = max_h;

    return 0;
}

void LayoutDraw(void *view)
{
    if (!view)
        return;

    Layout *layout = (Layout *)view;
    LayoutPosition *ly_positions = &layout->data.positions;

    if (layout->bg_color)
        GUI_DrawFillRectangle(ly_positions->x, ly_positions->y, ly_positions->w, ly_positions->h, layout->bg_color);

    LinkedListEntry *entry = LinkedListHead(layout->childs);

    while (entry)
    {
        ViewData *data = (ViewData *)LinkedListGetEntryData(entry);
        LayoutParam *params = &data->params;
        LayoutPosition *positions = &data->positions;

        if (positions->x >= ly_positions->x + ly_positions->w || positions->y >= ly_positions->y + ly_positions->h)
            break;

        if (params->draw)
            params->draw(data);

        entry = LinkedListNext(entry);
    }
}

int LayoutRefresh(Layout *layout)
{
    if (!layout)
        return -1;

    LayoutPosition *positions = &layout->data.positions;

    if (positions->layout_w > 0 && positions->layout_h > 0)
        return LayoutUpdate(layout, positions->x, positions->y, positions->layout_w, positions->layout_h);
    else if (layout->data.params.parent)
        return LayoutRefresh(layout->data.params.parent);
    else
        return LayoutUpdate(layout, positions->x, positions->y, positions->layout_max_w, positions->layout_max_w);
}

int LayoutAdd(Layout *layout, void *view)
{
    if (!layout || !view)
        return 0;

    ViewData *v_data = (ViewData *)view;
    LayoutParam *v_params = &v_data->params;
    v_params->parent = layout;
    v_params->layout_id = LinkedListAdd(layout->childs, view);

    return 1;
}

int LayoutRemove(Layout *layout, void *view)
{
    if (!layout || !view)
        return 0;

    ViewData *data = (ViewData *)view;
    LayoutParam *params = &data->params;
    LinkedListEntry *entry = (LinkedListEntry *)params->layout_id;
    if (params->parent != layout || !entry)
        return 0;

    if (params->destroy)
        params->destroy(view);

    LinkedListRemove(layout->childs, entry);

    return 1;
}

Layout *LayoutCreat(int orientation, int layout_w, int layout_h, int donot_free)
{
    Layout *layout = (Layout *)calloc(1, sizeof(Layout));
    if (!layout)
        return NULL;

    layout->childs = LinkedListCreat();
    if (!layout->childs)
    {
        free(layout);
        return NULL;
    }

    LayoutParam *params = &layout->data.params;
    LayoutPosition *positions = &layout->data.positions;
    layout->orientation = orientation;
    positions->layout_w = layout_w;
    positions->layout_h = layout_h;
    params->donot_free = donot_free;
    params->update = LayoutUpdate;
    params->draw = LayoutDraw;
    params->destroy = LayoutDestroy;

    return layout;
}
