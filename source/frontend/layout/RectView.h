#ifndef __M_RECT_VIEW_H__
#define __M_RECT_VIEW_H__

#include "Layout.h"

typedef struct
{
    LayoutParam params;
    uint32_t rect_color;
    uint32_t bg_color;
} RectView;

int RectViewUpdate(void *view, int remaining_w, int remaining_h);
void RectViewDraw(void *view, int x, int y);

int RectViewSetBgColor(RectView *rectView , uint32_t color);
int RectViewSetRectColor(RectView *rectView , uint32_t color);

int RectViewInit(RectView *rectView);
RectView *NewRectView();

#endif
