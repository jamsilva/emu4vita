#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "text_list.h"

void TextListFreeEntry(TextListEntry *entry)
{
    if (!entry)
        return;

    if (entry->text)
        free(entry->text);
    free(entry);
}

TextListEntry *TextListGetEntryByNumber(TextList *list, int n)
{
    if (!list)
        return NULL;

    TextListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

int TextListRemoveEntry(TextList *list, TextListEntry *entry)
{
    if (!list || !entry)
        return 0;

    if (entry->previous)
    {
        entry->previous->next = entry->next;
    }
    else
    {
        list->head = entry->next;
    }

    if (entry->next)
    {
        entry->next->previous = entry->previous;
    }
    else
    {
        list->tail = entry->previous;
    }

    TextListFreeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

void TextListAddEntry(TextList *list, TextListEntry *entry)
{
    if (!list || !entry)
        return;

    entry->next = NULL;
    entry->previous = NULL;

    if (list->head == NULL)
    {
        list->head = entry;
        list->tail = entry;
    }
    else
    {
        TextListEntry *tail = list->tail;
        tail->next = entry;
        entry->previous = tail;
        list->tail = entry;
    }

    list->length++;
}

void TextListAddEntryEx(TextList *list, const char *string)
{
    if (!list)
        return;

    TextListEntry *entry = (TextListEntry *)malloc(sizeof(TextListEntry));
    memset(entry, 0, sizeof(TextListEntry));
    if (string)
    {
        entry->text = (char *)malloc(strlen(string) + 1);
        if (entry->text)
            strcpy(entry->text, string);
    }

    TextListAddEntry(list, entry);
}

void TextListEmpty(TextList *list)
{
    if (!list)
        return;

    TextListEntry *entry = list->head;

    while (entry)
    {
        TextListEntry *next = entry->next;
        TextListFreeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}
