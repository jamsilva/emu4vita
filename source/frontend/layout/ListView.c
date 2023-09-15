#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "gui/gui.h"
#include "Layout.h"
#include "ListView.h"
#include "ItemView.h"
#include "utils.h"

static void ListViewFreeItemEntryData(void *data)
{
    if (!data)
        return;

    ListViewItemData *itemData = (ListViewItemData *)data;
    if (itemData->textView)
        itemData->textView->params.destroy(itemData->textView);
    free(itemData);
}

void ListViewDestroy(void *view)
{
    ListView *listView = (ListView *)view;

    if (!listView || listView->params.dont_free)
        return;

    LinkedListDestroy(listView->items);
    free(listView);
}

int ListViewUpdate(void *view, int max_w, int max_h)
{
    if (!view)
        return -1;

    ListView *listView = (ListView *)view;
    LayoutParam *params = &listView->params;

    max_w -= (params->margin_left + params->margin_right);
    max_h -= (params->margin_top + params->margin_bottom);

    int item_max_w = max_w - params->padding_left - params->padding_right;
    int item_max_h = max_h - params->padding_top - params->padding_bottom;
    int item_w = 0;
    int item_h = 0;
    listView->wrap_w = 0;
    listView->wrap_h = 0;

    LinkedListEntry *entry = LinkedListHead(listView->items);
    while (entry)
    {
        ListViewItemData *data = (ListViewItemData *)LinkedListGetEntryData(entry);

        LayoutParamSetLayoutSize(data->textView, TYPE_LAYOUT_MATH_PARENT, TYPE_LAYOUT_WRAP_CONTENT);
        LayoutParamSetMargin(data->textView, listView->itemView_margin_left, listView->itemView_margin_right, listView->itemView_margin_top, listView->itemView_margin_bottom);
        LayoutParamSetPadding(data->textView, listView->itemView_padding_left, listView->itemView_padding_right, listView->itemView_padding_top, listView->itemView_padding_bottom);
        TextViewSetSingleLine(data->textView, 1);
        TextViewUpdate(data->textView, item_max_w, item_max_h);
        item_w = data->textView->params.render_w + data->textView->params.margin_left + data->textView->params.margin_right;
        item_h = data->textView->params.render_h + data->textView->params.margin_top + data->textView->params.margin_bottom + listView->driver_size;
        if (listView->wrap_w < item_w)
            listView->wrap_w = item_w;
        listView->wrap_h += item_h;

        entry = LinkedListNext(entry);
    }
    if (listView->wrap_h > 0)
        listView->wrap_h -= listView->driver_size;

    params->render_w = params->layout_w;
    params->render_h = params->layout_h;

    if (params->layout_w == TYPE_LAYOUT_MATH_PARENT)
        params->render_w = max_w;
    else if (params->layout_w == TYPE_LAYOUT_WRAP_CONTENT)
        params->render_w = listView->wrap_w;
    if (params->render_w > max_w)
        params->render_w = max_w;
    if (params->render_w < 0)
        params->render_w = 0;

    if (params->layout_h == TYPE_LAYOUT_MATH_PARENT)
        params->render_h = max_h;
    else if (params->layout_h == TYPE_LAYOUT_WRAP_CONTENT)
        params->render_h = listView->wrap_h;
    if (params->render_h > max_h)
        params->render_h = max_h;
    if (params->render_h < 0)
        params->render_h = 0;

    listView->n_draw_items = (item_max_h + listView->driver_size) / item_h;

    return 0;
}

void ListViewDraw(void *view, int x, int y)
{
    if (!view)
        return;

    ListView *listView = (ListView *)view;
    LayoutParam *params = &listView->params;

    if (params->render_w <= 0 || params->render_h <= 0)
        return;

    ListViewCallbacks *callbacks = &listView->callbacks;
    int layout_x = x + params->margin_left;
    int layout_y = y + params->margin_top;

    if (listView->bg_color)
        GUI_DrawFillRectangle(layout_x, layout_y, params->render_w, params->render_h, listView->bg_color);

    int child_min_x = layout_x + params->padding_left;
    int child_min_y = layout_y + params->padding_top;
    int child_max_x = layout_x + params->render_w - params->padding_right;
    int child_max_y = layout_y + params->render_h - params->padding_bottom;
    int child_max_w = child_max_x - child_min_x;
    int child_max_h = child_max_y - child_min_y;

    GUI_EnableClipping(child_min_x, child_min_y, child_max_w, child_max_h);

    int child_x = child_min_x;
    int child_y = child_min_y;

    if (listView->items)
    {
        int n_items = LinkedListGetLength(listView->items);
        LinkedListEntry *entry = LinkedListFindByNum(listView->items, listView->top_pos);

        int i;
        for (i = listView->top_pos; entry; i++)
        {
            if (child_x >= child_max_x || child_y >= child_max_y)
                break;

            ListViewItemData *data = (ListViewItemData *)LinkedListGetEntryData(entry);

            uint32_t text_color = COLOR_WHITE;
            if (callbacks->getNameColor)
                text_color = callbacks->getNameColor(listView->list, data->entry, data->index);
            TextViewSetTextColor(data->textView, text_color);

            if (listView->focus_pos_enable && i == listView->focus_pos)
            {
                TextViewSetTextScollEnabled(data->textView, 1);
                TextViewSetBgColor(data->textView, listView->item_focus_color);
            }
            else
            {
                TextViewSetTextScollEnabled(data->textView, 0);
                TextViewSetBgColor(data->textView, listView->item_bg_color);
            }

            TextViewDraw(data->textView, child_x, child_y);

            child_y += (data->textView->params.render_h + data->textView->params.margin_top + data->textView->params.margin_bottom);
            if (listView->driver_size > 0)
            {
                GUI_DrawFillRectangle(child_x, child_y, child_max_w, listView->driver_size, listView->driver_color);
                child_y += listView->driver_size;
            }

            entry = LinkedListNext(entry);
        }

        // Scrollbar
        int scrollbar_track_x = child_max_x - GUI_DEF_SCROLLBAR_SIZE;
        int scrollbar_track_y = child_min_y;
        int scrollbar_track_height = child_max_h;
        GUI_DrawVerticalScrollbar(scrollbar_track_x, scrollbar_track_y, scrollbar_track_height, n_items,
                                  listView->n_draw_items, listView->top_pos, 0);
    }

    GUI_DisableClipping();
}

int ListViewRemoveItem(ListView *listView, int n)
{
    if (!listView || !listView->items)
        return 0;

    LinkedListEntry *entry = LinkedListFindByNum(listView->items, n);
    if (!entry)
        return 0;

    return LinkedListRemove(listView->items, entry);
}

int ListViewEmptyItems(ListView *listView)
{
    if (!listView || !listView->items)
        return -1;

    LinkedListEmpty(listView->items);

    return 0;
}

int ListViewRefreshtList(ListView *listView)
{
    if (!listView || !listView->items)
        return -1;

    ListViewEmptyItems(listView);

    if (!listView->list)
        return -1;

    ListViewCallbacks *callbacks = &listView->callbacks;
    void *entry = NULL;
    if (callbacks->getHeadListEntry)
        entry = callbacks->getHeadListEntry(listView->list);

    int i;
    for (i = 0; entry; i++)
    {
        ListViewItemData *data = calloc(1, sizeof(ListViewItemData));
        if (!data)
            goto NEXT;

        data->textView = NewTextView();
        if (!data->textView)
        {
            free(data);
            goto NEXT;
        }

        data->entry = entry;
        data->index = i;

        char *name = NULL;
        if (callbacks->getName)
            name = callbacks->getName(listView->list, entry, i);
        TextViewSetText(data->textView, name);

        LinkedListAdd(listView->items, data);

    NEXT:
        if (!callbacks->getNextListEntry)
            break;
        entry = callbacks->getNextListEntry(listView->list, entry, i);
    }

    ListViewRefreshPos(listView);
    ViewRefresh(listView);

    return 0;
}

int ListViewSetList(ListView *listView, void *list, ListViewCallbacks *callbacks)
{
    if (!listView)
        return -1;

    listView->list = list;
    if (callbacks)
        memcpy(&listView->callbacks, callbacks, sizeof(ListViewCallbacks));
    else
        memset(&listView->callbacks, 0, sizeof(ListViewCallbacks));

    ListViewRefreshtList(listView);

    return 0;
}

int ListViewGetTopPos(ListView *listView)
{
    if (!listView)
        return -1;

    return listView->top_pos;
}

int ListViewSetTopPos(ListView *listView, int pos)
{
    if (!listView)
        return -1;

    listView->top_pos = pos;
    ListViewRefreshPos(listView);

    return 0;
}

int ListViewGetFocusPos(ListView *listView)
{
    if (!listView)
        return -1;

    return listView->focus_pos;
}

int ListViewSetFocusPos(ListView *listView, int pos)
{
    if (!listView)
        return -1;

    listView->focus_pos = pos;
    ListViewRefreshPos(listView);

    return 0;
}

int ListViewSetFocusPosEnabled(ListView *listView, int enable)
{
    if (!listView)
        return -1;

    listView->focus_pos_enable = enable;

    return 0;
}

int ListViewRefreshPos(ListView *listView)
{
    if (!listView || !listView->items)
        return -1;

    int n_items = LinkedListGetLength(listView->items);
    if (listView->focus_pos_enable)
        RefreshListPos(&listView->top_pos, &listView->focus_pos, n_items, listView->n_draw_items);
    else
        RefreshListPosNoFocus(&listView->top_pos, n_items, listView->n_draw_items);

    return 0;
}

int ListViewMovePos(ListView *listView, int move_type)
{
    if (!listView || !listView->items)
        return -1;

    int n_items = LinkedListGetLength(listView->items);
    if (listView->focus_pos_enable)
        MoveListPos(move_type, &listView->top_pos, &listView->focus_pos, n_items, listView->n_draw_items);
    else
        MoveListPosNoFocus(move_type, &listView->top_pos, n_items, listView->n_draw_items);

    return 0;
}

int ListViewSetChooseMode(ListView *listView, int mode)
{
    if (!listView)
        return -1;

    listView->choose_mode = mode;

    return 0;
}

int ListViewSetCheckBoxShowwing(ListView *listView, int showwing)
{
    if (!listView)
        return -1;

    listView->checkBox_showwing = showwing;

    return 0;
}

int ListViewSetDriverSize(ListView *listView, int size)
{
    if (!listView)
        return -1;

    listView->driver_size = size;

    return 0;
}

int ListViewSetBgColor(ListView *listView, uint32_t color)
{
    if (!listView)
        return -1;

    listView->bg_color = color;

    return 0;
}

int ListViewSetDriverColor(ListView *listView, uint32_t color)
{
    if (!listView)
        return -1;

    listView->driver_color = color;

    return 0;
}

int ListViewSetItemMargin(ListView *listView, int left, int right, int top, int bottom)
{
    if (!listView)
        return -1;

    listView->itemView_margin_left = left;
    listView->itemView_margin_right = right;
    listView->itemView_margin_top = top;
    listView->itemView_margin_bottom = bottom;

    return 0;
}

int ListViewSetItemPadding(ListView *listView, int left, int right, int top, int bottom)
{
    if (!listView)
        return -1;

    listView->itemView_padding_left = left;
    listView->itemView_padding_right = right;
    listView->itemView_padding_top = top;
    listView->itemView_padding_bottom = bottom;

    return 0;
}

int ListViewSetItemBgColor(ListView *listView, uint32_t color)
{
    if (!listView)
        return -1;

    listView->item_bg_color = color;

    return 0;
}

int ListViewSetItemFocusColor(ListView *listView, uint32_t color)
{
    if (!listView)
        return -1;

    listView->item_focus_color = color;

    return 0;
}

int ListViewSetItemNameColor(ListView *listView, uint32_t color)
{
    if (!listView)
        return -1;

    listView->item_name_color = color;

    return 0;
}

int ListViewIsItemSelected(ListView *listView, int id)
{
    if (!listView || !listView->items)
        return 0;

    LinkedListEntry *entry = LinkedListFindByNum(listView->items, id);
    if (!entry)
        return 0;

    ListViewItemData *data = (ListViewItemData *)LinkedListGetEntryData(entry);
    return data->is_selected;
}

int ListViewSetItemSelected(ListView *listView, int id, int selected)
{
    if (!listView || !listView->items)
        return -1;

    LinkedListEntry *entry = LinkedListFindByNum(listView->items, id);
    if (!entry)
        return -1;

    ListViewItemData *data = (ListViewItemData *)LinkedListGetEntryData(entry);
    data->is_selected = selected;

    return 0;
}

int ListViewResetItemsSelected(ListView *listView)
{
    if (!listView || !listView->items)
        return -1;

    LinkedListEntry *entry = LinkedListHead(listView->items);
    while (entry)
    {
        ListViewItemData *data = (ListViewItemData *)LinkedListGetEntryData(entry);
        data->is_selected = 0;
        entry = LinkedListNext(entry);
    }

    return 0;
}

int ListViewInit(ListView *listView)
{
    if (!listView)
        return -1;

    memset(listView, 0, sizeof(ListView));

    listView->items = NewLinkedList();
    if (!listView->items)
        return -1;
    LinkedListSetFreeEntryDataCallback(listView->items, ListViewFreeItemEntryData);

    LayoutParam *params = &listView->params;
    params->destroy = ListViewDestroy;
    params->update = ListViewUpdate;
    params->draw = ListViewDraw;

    return 0;
}

ListView *NewListView()
{
    ListView *listView = (ListView *)malloc(sizeof(ListView));
    if (!listView)
        return NULL;

    if (ListViewInit(listView) < 0)
    {
        free(listView);
        return NULL;
    }

    return listView;
}
