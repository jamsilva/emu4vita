#ifndef __M_TEXT_VIEW_H__
#define __M_TEXT_VIEW_H__

typedef struct TextView
{
    ViewData data;
    char *text;
    char *short_text;
    int text_w;
    int text_h;
    int text_x;
    int text_y;
    uint32_t text_color;
    uint32_t bg_color;
} TextView;

#endif
