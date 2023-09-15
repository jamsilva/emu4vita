#ifndef __M_OVERLAY_LIST_H__
#define __M_OVERLAY_LIST_H__

#include "linked_list.h"

typedef struct
{
    char *name;
    char *image_name;
    int *viewport_rotate;
    int *viewport_width;
    int *viewport_height;
    int *viewport_x;
    int *viewport_y;
} OverlayListEntryData;

LinkedListEntry *ConfigListFindEntryByName(LinkedList *list, const char *name);
int OverlayListGetEntriesFromBuffer(LinkedList *list, void *buffer, int size);
int OverlayListGetEntries(LinkedList *list, const char *path);
LinkedList * NewOverlayList();

#endif