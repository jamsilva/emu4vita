#ifndef __M_STRING_LIST_H__
#define __M_STRING_LIST_H__

typedef struct StringListEntry
{
    struct StringListEntry *next;
    struct StringListEntry *prev;
    char *string;
} StringListEntry;

typedef struct
{
    StringListEntry *head;
    StringListEntry *tail;
    int length;
} StringList;

StringListEntry *StringListGetEntryByNumber(StringList *list, int n);
int StringListRemoveEntry(StringList *list, StringListEntry *entry);
void StringListAddEntry(StringList *list, StringListEntry *entry);
void StringListAdd(StringList *list, const char *string);
void StringListEmpty(StringList *list);

#endif