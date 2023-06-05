#ifndef __M_OVERLAY_LIST_H__
#define __M_OVERLAY_LIST_H__

typedef struct OverlayListEntry
{
    struct OverlayListEntry *next;
    struct OverlayListEntry *previous;
    char *name;
    char *image_name;
    int *viewport_rotate;
    int *viewport_width;
    int *viewport_height;
    int *viewport_x;
    int *viewport_y;
} OverlayListEntry;

typedef struct
{
    OverlayListEntry *head;
    OverlayListEntry *tail;
    int length;
    int sel_pos;
} OverlayList;

OverlayListEntry *OverlayListGetEntryByNumber(OverlayList *list, int n);
OverlayListEntry *OverlayListGetEntryByName(OverlayList *list, const char *name);
int OverlayListGetNumberByName(OverlayList *list, const char *name);

int OverlayListRemoveEntry(OverlayList *list, OverlayListEntry *entry);
void OverlayListAddEntry(OverlayList *list, OverlayListEntry *entry);
void OverlayListEmpty(OverlayList *list);

int OverlayListGetEntriesFromBuffer(OverlayList *list, void *buffer, int size);
int OverlayListGetEntries(OverlayList *list, const char *path);

#endif