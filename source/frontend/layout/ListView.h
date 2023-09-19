#ifndef __M_LIST_VIEW_H__
#define __M_LIST_VIEW_H__

#include "list/linked_list.h"
#include "Layout.h"
#include "TextView.h"

typedef struct
{
    TextView *textView;
    int x, y;
    void *entry;
    int index;
    int is_selected;
} ListViewItemData;

typedef struct
{
    int (*getListLength)(void *list);
    void *(*getHeadListEntry)(void *list);
    void *(*getTailListEntry)(void *list);
    void *(*getNextListEntry)(void *list, void *cur_entry, int cur_idx);
    void *(*getPrevListEntry)(void *list, void *cur_entry, int cur_idx);

    GUI_Texture *(*getImage)(void *list, void *entry, int idx);
    char *(*getName)(void *list, void *entry, int idx);
    char *(*getInfo)(void *list, void *entry, int idx);

    uint32_t (*getNameColor)(void *list, void *entry, int idx);
    uint32_t (*getInfoColor)(void *list, void *entry, int idx);

    char *(*onItemClick)(void *list, void *cur_entry, int cur_idx);
    char *(*onItemLongClick)(void *list, void *cur_entry, int cur_idx);
} ListViewCallbacks;

typedef struct
{
    LayoutParam params;
    int orientation;
    LinkedList *items;
    int itemView_margin_left;
    int itemView_margin_right;
    int itemView_margin_top;
    int itemView_margin_bottom;
    int itemView_padding_left;
    int itemView_padding_right;
    int itemView_padding_top;
    int itemView_padding_bottom;
    int driver_size;
    uint32_t driver_color;
    uint32_t item_name_color;
    uint32_t item_focus_color;
    uint32_t item_bg_color;
    uint32_t bg_color;
    int checkBox_showwing;
    int choose_mode;
    void *list;
    int top_pos;
    int focus_pos;
    int focus_pos_enable;
    int n_draw_items;
    ListViewCallbacks callbacks;
} ListView;

void ListViewDestroy(void *view);
int ListViewUpdate(void *view, int remaining_w, int remaining_h);
void ListViewDraw(void *view, int x, int y);

int ListViewRemoveItem(ListView *listView, int n);
int ListViewEmptyItems(ListView *listView);
int ListViewRefreshtList(ListView *listView);
int ListViewSetList(ListView *listView, void *list, ListViewCallbacks *callbacks);

int ListViewGetTopPos(ListView *listView);
int ListViewSetTopPos(ListView *listView, int pos);
int ListViewGetFocusPos(ListView *listView);
int ListViewSetFocusPos(ListView *listView, int pos);
int ListViewSetFocusPosEnabled(ListView *listView, int enable);
int ListViewRefreshPos(ListView *listView);
int ListViewMovePos(ListView *listView, int move_type);

int ListViewSetChooseMode(ListView *listView, int choose_mode);
int ListViewSetCheckBoxShowwing(ListView *listView, int enable);
int ListViewSetDriverSize(ListView *listView, int size);
int ListViewSetBgColor(ListView *listView, uint32_t color);
int ListViewSetDriverColor(ListView *listView, uint32_t color);

int ListViewSetItemMargin(ListView *listView, int left, int right, int top, int bottom);
int ListViewSetItemPadding(ListView *listView, int left, int right, int top, int bottom);
int ListViewSetItemBgColor(ListView *listView, uint32_t color);
int ListViewSetItemFocusColor(ListView *listView, uint32_t color);
int ListViewSetItemNameColor(ListView *listView, uint32_t color);

int ListViewIsItemSelected(ListView *listView, int id);
int ListViewSetItemSelected(ListView *listView, int id, int selected);
int ListViewResetItemsSelected(ListView *listView);

int ListViewInit(ListView *listView);
ListView *NewListView();

#endif
