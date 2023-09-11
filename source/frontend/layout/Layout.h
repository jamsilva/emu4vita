#ifndef __M_LAYOUT_H__
#define __M_LAYOUT_H__

#include "list/linked_list.h"

enum TypeLayoutSize
{
    TYPE_LAYOUT_WRAP_CONTENT = -0x01,
    TYPE_LAYOUT_MATH_PARENT = -0x02,
};

enum TypeLayoutOrientation
{
    TYPE_LAYOUT_ORIENTATION_HORIZONTAL,
    TYPE_LAYOUT_ORIENTATION_VERTICAL,
};

typedef struct Layout Layout;

typedef struct
{
    Layout *parent;
    void *layout_id;
    int donot_free;
    int (*update)(void *view, int x, int y, int max_w, int max_h);
    void (*draw)(void *view);
    void (*destroy)(void *view);
} LayoutParam;

typedef struct
{
    int layout_w;
    int layout_h;
    int layout_max_w;
    int layout_max_h;
    int x, y;
    int w, h;
    int padding_left;
    int padding_right;
    int padding_top;
    int padding_bottom;
    int margin_left;
    int margin_right;
    int margin_top;
    int margin_bottom;
} LayoutPosition;

typedef struct
{
    LayoutParam params;
    LayoutPosition positions;
} ViewData;

struct Layout
{
    ViewData data;
    int orientation;
    int remaining_x;
    int remaining_y;
    int remaining_w;
    int remaining_h;
    uint32_t bg_color;
    LinkedList *childs;
};

void LayoutDestroy(void *view);
int LayoutUpdate(void *view, int x, int y, int max_w, int max_h);
void LayoutDraw(void *view);

int LayoutRefresh(Layout *layout);
int LayoutAdd(Layout *layout, void *view);
int LayoutRemove(Layout *layout, void *view);
Layout *LayoutCreat(int orientation, int layout_w, int layout_h, int donot_free);

#endif
