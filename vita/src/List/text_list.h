#ifndef __M_TEXT_LIST_H__
#define __M_TEXT_LIST_H__

typedef struct TextListEntry
{
    struct TextListEntry *next;
    struct TextListEntry *previous;
    char *text;
} TextListEntry;

typedef struct
{
    TextListEntry *head;
    TextListEntry *tail;
    int length;
} TextList;

void TextListFreeEntry(TextListEntry *entry);
TextListEntry *TextListGetEntryByNumber(TextList *list, int n);
int TextListRemoveEntry(TextList *list, TextListEntry *entry);
void TextListAddEntry(TextList *list, TextListEntry *entry);
void TextListAddEntryEx(TextList *list, const char *string);
void TextListEmpty(TextList *list);

#endif