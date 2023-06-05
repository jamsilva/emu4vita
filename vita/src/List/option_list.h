#ifndef __M_OPTION_LIST_H__
#define __M_OPTION_LIST_H__

typedef struct
{
    char *value;
    char *label;
} OptionValue;

typedef struct OptionListEntry
{
    struct OptionListEntry *next;
    struct OptionListEntry *previous;
    char *key;
    char *desc;
    OptionValue *values;
    int n_values;
    char *default_value;
    int *visible;
    uint32_t sel_pos;
} OptionListEntry;

typedef struct
{
    OptionListEntry *head;
    OptionListEntry *tail;
    int length;
} OptionList;

void OptionListFreeEntry(OptionListEntry *entry);
OptionListEntry *OptionListGetEntryByNumber(OptionList *list, int n);
OptionListEntry *OptionListGetEntryByKey(OptionList *list, char *key);
int OptionListRemoveEntry(OptionList *list, OptionListEntry *entry);
void OptionListAddEntry(OptionList *list, OptionListEntry *entry);
void OptionListEmpty(OptionList *list);
int OptionListSaveConfig(OptionList *list, const char *path);

#endif