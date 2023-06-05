#ifndef __M_GRAPHICS_H__
#define __M_GRAPHICS_H__

int initUiFonts();
void finishUiFonts();

int UiDrawText(int x, int y, unsigned int color, const char *text);
int UiDrawTextf(int x, int y, unsigned int color, const char *text, ...);

float UiGetFontHeight();
float UiGetLineHeight();

int UiGetTextWidth(const char *text);
int UiGetTextHeight(const char *text);

void UiStartDrawing();
void UiEndDrawing();

void vita2d_draw_empty_rectangle(float x, float y, float w, float h, float l, unsigned int color);

#endif
