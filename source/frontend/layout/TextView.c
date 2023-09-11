#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"
#include "TextView.h"

void ImageViewDestroy(void *view)
{
    TextView *textView = (TextView *)view;

    if (!textView || textView->data.params.donot_free)
        return;

    if (textView->text)
        free(textView->text);
    if (textView->text)
        free(textView->text);
    free(textView);
}

int ImageViewUpdate(void *view, int x, int y, int max_w, int max_h)
{
    if (!view)
        return -1;

    TextView *textView = (TextView *)view;
    LayoutPosition *positions = &textView->data.positions;

    positions->x = x;
    positions->y = y;
    positions->w = positions->layout_w;
    positions->h = positions->layout_h;

    if (positions->layout_w == TYPE_LAYOUT_MATH_PARENT)
    {
        positions->w = max_w;
    }
    else if (positions->layout_w == TYPE_LAYOUT_WRAP_CONTENT)
    {
        if (textView->text)
            positions->w = GUI_GetTextWidth(textView->text);
        else
            positions->w = 0;
    }
    if (positions->w > max_w)
        positions->w = max_w;

    if (positions->layout_h == TYPE_LAYOUT_MATH_PARENT)
    {
        positions->h = max_h;
    }
    else if (positions->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
    {
        if (textView->text)
            positions->w = GUI_GetTextHeight(textView->text);
        else
            positions->h = 0;
    }
    if (positions->h > max_h)
        positions->h = max_h;

    if (textView->text)
    {
    }

    return 0;
}

void TextViewDraw(void *view)
{
    if (!view)
        return;

    TextView *textView = (TextView *)view;
    LayoutPosition *positions = &textView->data.positions;

    if (textView->bg_color)
        GUI_DrawFillRectangle(positions->x, positions->y, positions->w, positions->h, textView->bg_color);

    if (textView->text)
    {
        int x = positions->x + positions->padding_left + textView->text_x;
        int y = positions->y + positions->padding_top + textView->text_y;
        GUI_DrawText(x, y, textView->text_color, textView->text);
    }
}

int TextViewRefresh(TextView *view)
{
    if (!view)
        return -1;

    LayoutPosition *positions = &view->data.positions;

    if (positions->layout_w > 0 && positions->layout_h > 0)
        return TextViewUpdate(view, positions->x, positions->y, positions->layout_w, positions->layout_h);
    else if (view->data.params.parent)
        return LayoutRefresh(view->data.params.parent);
    else
        return TextViewUpdate(view, positions->x, positions->y, positions->layout_max_w, positions->layout_max_h);
}

int TextViewSetText(TextView *view, const char *text)
{
    if (!view || !text)
        return -1;

    if (view->text)
        free(view->text);
    view->text = NULL;
    if (text)
    {
        view->text = malloc(strlen(text) + 1);
        if (view->text)
        {
            strcpy(view->text, text);
            view->text_w = GUI_GetTextWidth(view->text);
            view->text_h = GUI_GetTextHeight(view->text);
        }
    }

    return 0;
}

TextView *TextViewViewCreat(int layout_w, int layout_h, int donot_free)
{
    TextView *view = (TextView *)calloc(1, sizeof(TextView));
    if (!view)
        return NULL;

    LayoutParam *params = &view->data.params;
    LayoutPosition *positions = &view->data.positions;

    positions->layout_w = layout_w;
    positions->layout_h = layout_h;
    params->donot_free = donot_free;
    params->update = TextViewUpdate;
    params->draw = TextViewDraw;
    params->destroy = TextViewDestroy;

    return view;
}
