#ifndef __M_ITEM_VIEW_H__
#define __M_ITEM_VIEW_H__

#include "Layout.h"

typedef struct
{
    LayoutParam params;
    GUI_Texture *image;
    int image_auto_free;
    char *name;
    char *info;
    int is_selected;
    int is_focus;
    uint32_t bg_color;
} ItemView;

void ItemViewDestroy(void *view);
int ItemViewUpdate(void *view, int remaining_w, int remaining_h);
void ItemViewDraw(void *view, int x, int y);

int ItemViewSetImage(ItemView *itemView, GUI_Texture *image, int auto_free);
int ItemViewSetName(ItemView *itemView, const char *name, int copy);
int ItemViewSetInfo(ItemView *itemView, const char *name, int copy);
int ItemViewSetSelected(ItemView *itemView, int selected);
int ItemViewSetFocus(ItemView *itemView, int focus);
int ItemViewSetChooseMode(ItemView *itemView, int mode);
int ItemViewSetCheckBoxShowEnable(ItemView *itemView, int enable);

int ItemViewInit(ItemView *itemView);
ItemView *NewItemView();

#endif