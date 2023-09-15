#ifndef __M_OPTION_LIST_H__
#define __M_OPTION_LIST_H__

#include "linked_list.h"

typedef struct
{
    char *value;
    char *label;
} OptionValue;

typedef struct OptionListEntryData
{
    char *key;
    char *desc;
    OptionValue *values;
    int n_values;
    int visibility;
    uint32_t select;
    uint32_t default_select;
} OptionListEntryData;

LinkedListEntry *OptionListFindEntryByKey(LinkedList *list, const char *key);
int OptionListResetConfig(LinkedList *list);
int OptionListLoadConfig(LinkedList *list, const char *path);
int OptionListSaveConfig(LinkedList *list, const char *path);
LinkedList *NewOptionList();

#endif