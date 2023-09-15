#ifndef __M_CONFIG_LIST_H__
#define __M_CONFIG_LIST_H__

#include "linked_list.h"

typedef struct ConfigListEntryData
{
    char *key;
    char *value;
} ConfigListEntryData;

LinkedListEntry *ConfigListFindEntryByKey(LinkedList *list, const char *key);
int ConfigListGetEntriesFromBuffer(LinkedList *list, void *buffer, int size);
int ConfigListGetEntries(LinkedList *list, const char *path);
LinkedList *NewConfigList();

#endif