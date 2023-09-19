#ifndef __M_TEXT_VIEW_H__
#define __M_TEXT_VIEW_H__

#include "Layout.h"

typedef struct
{
    LayoutParam params;
    char *text;
    char *text_buf;
    int text_w;
    int text_h;
    int text_x;
    int text_y;
    uint32_t text_color;
    uint32_t bg_color;
    int single_line;
    int text_scoll_enable;
    int text_scoll_count;
    int text_x_off;
} TextView;

void TextViewDestroy(void *view);
int TextViewUpdate(void *view, int remaining_w, int remaining_h);
void TextViewDraw(void *view, int x, int y);

int TextViewSetText(TextView *textView, const char *text);
int TextViewSetBgColor(TextView *textView, uint32_t color);
int TextViewSetTextColor(TextView *textView, uint32_t color);
int TextViewSetSingleLine(TextView *textView, int single_line);
int TextViewSetTextScollEnabled(TextView *textView, int enable);

int TextViewInit(TextView *textView);
TextView *NewTextView();

#endif
