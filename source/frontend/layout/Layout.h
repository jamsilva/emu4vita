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
    TYPE_LAYOUT_ORIENTATION_FRAME,
    TYPE_LAYOUT_ORIENTATION_HORIZONTAL,
    TYPE_LAYOUT_ORIENTATION_VERTICAL,
};

enum TypeCheckBox
{
    TYPE_CHOOSE_MODE_DISABLE,
    TYPE_CHOOSE_MODE_SINGLE,
    TYPE_CHOOSE_MODE_MULTI,
};

typedef struct Layout Layout;

typedef struct
{
    int measured_w;
    int measured_h;
    int layout_w;
    int layout_h;
    int padding_left;
    int padding_right;
    int padding_top;
    int padding_bottom;
    int margin_left;
    int margin_right;
    int margin_top;
    int margin_bottom;

    Layout *parent;
    void *above_view;
    int dont_free;
    int is_clicked;
    int is_focus;
    void (*destroy)(void *view);
    int (*update)(void *view, int remaining_w, int remaining_h);
    void (*draw)(void *view, int x, int y);
} LayoutParam;

struct Layout
{
    LayoutParam params;
    int orientation;
    int scroll_enable;
    int remaining_w;
    int remaining_h;
    LinkedList *childs;
    uint32_t bg_color;
};

void LayoutDestroy(void *view);
int LayoutUpdate(void *view, int remaining_w, int remaining_h);
void LayoutDraw(void *view, int x, int y);

int LayoutSetOrientation(Layout *layout, int orientation);
int LayoutSetBgColor(Layout *layout, uint32_t color);
int LayoutGetRemaining(Layout *layout, int *remaining_w, int *remaining_h);

int LayoutAdd(Layout *layout, void *view);
int LayoutRemove(Layout *layout, void *view);
void LayoutEmpty(Layout *layout);
int LayoutInit(Layout *layout);
Layout *NewLayout();

int LayoutParamSetMargin(void *view, int left, int right, int top, int bottom);
int LayoutParamSetPadding(void *view, int left, int right, int top, int bottom);
int LayoutParamSetLayoutSize(void *view, int layout_w, int layout_h);
int LayoutParamSetAutoFree(void *view, int auto_free);

int RootViewUpdate(void *view);
int ViewRefresh(void *view);
void ViewDestroy(void *view);
int ViewAddAbove(void *view, void *above);

#include "RectView.h"
#include "TextView.h"
#include "ImageView.h"
#include "ListView.h"

#endif
