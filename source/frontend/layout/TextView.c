#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"
#include "TextView.h"
#include "utils.h"
#include "utils_string.h"

void TextViewDestroy(void *view)
{
    TextView *textView = (TextView *)view;

    if (!textView || textView->params.dont_free)
        return;

    if (textView->text)
        free(textView->text);
    if (textView->text_buf)
        free(textView->text_buf);
    free(textView);
}

int TextViewUpdate(void *view, int remaining_w, int remaining_h)
{
    if (!view)
        return -1;

    TextView *textView = (TextView *)view;
    LayoutParam *params = &textView->params;

    int max_w = remaining_w - params->margin_left - params->margin_right;
    int max_h = remaining_h - params->margin_top - params->margin_bottom;
    int wrap_w = textView->text_w + params->padding_left + params->padding_right;
    int wrap_h = textView->text_h + params->padding_top + params->padding_bottom;

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

    if (textView->text_buf)
        free(textView->text_buf);
    textView->text_buf = NULL;

    if (textView->text)
    {
        int text_max_w = params->measured_w - params->padding_left - params->padding_right;

        if (textView->text_w > text_max_w)
        {
            if (textView->single_line)
                textView->text_buf = StringMakeShortByWidth(textView->text, text_max_w);
            else
                textView->text_buf = StringBreakLineByWidth(textView->text, text_max_w);
            if (params->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
                params->measured_h = GUI_GetTextHeight(textView->text_buf) + 4 + params->padding_top + params->padding_bottom;
        }
    }

    return 0;
}

void TextViewDraw(void *view, int x, int y)
{
    if (!view)
        return;

    TextView *textView = (TextView *)view;
    LayoutParam *params = &textView->params;

    if (params->measured_w <= 0 || params->measured_h <= 0)
        return;

    int view_x = x + params->margin_left;
    int view_y = y + params->margin_top;

    if (textView->bg_color)
        GUI_DrawFillRectangle(view_x, view_y, params->measured_w, params->measured_h, textView->bg_color);

    if (textView->text)
    {
        const char *text = textView->text_buf;
        int text_x = view_x + params->padding_left + textView->text_x;
        int text_y = view_y + params->padding_top + textView->text_y;
        int text_max_w = params->measured_w - params->padding_left - params->padding_right;
        int text_max_h = params->measured_h - params->padding_top - params->padding_bottom;
        uint32_t color = textView->text_color;

        GUI_EnableClipping(text_x, text_y, text_max_w, text_max_h);

        if (textView->text_scoll_enable && textView->text_w > text_max_w)
        {
            text = textView->text;

            if (textView->text_scoll_count < 60)
            {
                textView->text_x_off = 0;
            }
            else if (textView->text_scoll_count < textView->text_w + 60)
            {
                textView->text_x_off++;
            }
            else if (textView->text_scoll_count < textView->text_w + 90)
            {
                color = (color & 0x00FFFFFF) | ((((color >> 24) * (textView->text_scoll_count - textView->text_w - 60)) / 30) << 24); // fade-in in 0.5s
                textView->text_x_off = 0;
            }
            else
            {
                textView->text_scoll_count = 0;
            }

            textView->text_scoll_count++;
            text_x -= textView->text_x_off;
        }
        else
        {
            textView->text_scoll_count = 0;
            textView->text_x_off = 0;
        }

        if (!text)
            text = textView->text;
        if (text)
            GUI_DrawText(text_x, text_y, color, text);

        GUI_DisableClipping();
    }
}

int TextViewSetText(TextView *textView, const char *text)
{
    if (!textView)
        return -1;

    if (textView->text)
        free(textView->text);
    textView->text = NULL;
    if (textView->text_buf)
        free(textView->text_buf);
    textView->text_buf = NULL;
    textView->text_w = 0;
    textView->text_h = 0;

    if (text)
    {
        textView->text = malloc(strlen(text) + 1);
        if (textView->text)
        {
            strcpy(textView->text, text);
            textView->text_w = GUI_GetTextWidth(textView->text);
            textView->text_h = GUI_GetTextHeight(textView->text) + 4;
        }
    }

    return 0;
}

int TextViewSetBgColor(TextView *textView, uint32_t color)
{
    if (!textView)
        return -1;

    textView->bg_color = color;

    return 0;
}

int TextViewSetTextColor(TextView *textView, uint32_t color)
{
    if (!textView)
        return -1;

    textView->text_color = color;

    return 0;
}

int TextViewSetSingleLine(TextView *textView, int single_line)
{
    if (!textView)
        return -1;

    textView->single_line = single_line;

    return 0;
}

int TextViewSetTextScollEnabled(TextView *textView, int enable)
{
    if (!textView)
        return -1;

    textView->text_scoll_enable = enable;

    return 0;
}

int TextViewInit(TextView *textView)
{
    if (!textView)
        return -1;

    memset(textView, 0, sizeof(TextView));

    LayoutParam *params = &textView->params;
    params->destroy = TextViewDestroy;
    params->update = TextViewUpdate;
    params->draw = TextViewDraw;

    return 0;
}

TextView *NewTextView()
{
    TextView *textView = (TextView *)malloc(sizeof(TextView));
    if (!textView)
        return NULL;

    TextViewInit(textView);

    return textView;
}
