#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "string_list.h"

static void freeEntry(StringListEntry *entry)
{
    if (!entry)
        return;

    if (entry->string)
        free(entry->string);
    free(entry);
}

StringListEntry *StringListGetEntryByNumber(StringList *list, int n)
{
    if (!list)
        return NULL;

    StringListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

int StringListRemoveEntry(StringList *list, StringListEntry *entry)
{
    if (!list || !entry)
        return 0;

    if (entry->prev)
    {
        entry->prev->next = entry->next;
    }
    else
    {
        list->head = entry->next;
    }

    if (entry->next)
    {
        entry->next->prev = entry->prev;
    }
    else
    {
        list->tail = entry->prev;
    }

    freeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

void StringListAddEntry(StringList *list, StringListEntry *entry)
{
    if (!list || !entry)
        return;

    entry->next = NULL;
    entry->prev = NULL;

    if (list->head == NULL)
    {
        list->head = entry;
        list->tail = entry;
    }
    else
    {
        StringListEntry *tail = list->tail;
        tail->next = entry;
        entry->prev = tail;
        list->tail = entry;
    }

    list->length++;
}

void StringListAdd(StringList *list, const char *string)
{
    if (!list)
        return;

    StringListEntry *entry = (StringListEntry *)malloc(sizeof(StringListEntry));
    memset(entry, 0, sizeof(StringListEntry));
    if (string)
    {
        entry->string = (char *)malloc(strlen(string) + 1);
        if (entry->string)
            strcpy(entry->string, string);
    }

    StringListAddEntry(list, entry);
}

void StringListEmpty(StringList *list)
{
    if (!list)
        return;

    StringListEntry *entry = list->head;

    while (entry)
    {
        StringListEntry *next = entry->next;
        freeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}
