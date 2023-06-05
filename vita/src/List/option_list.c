#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "option_list.h"

void OptionListFreeEntry(OptionListEntry *entry)
{
    if (!entry)
        return;

    if (entry->key)
        free(entry->key);
    if (entry->desc)
        free(entry->desc);
    if (entry->default_value)
        free(entry->default_value);
    if (entry->values)
    {
        int i;
        for (i = 0; i < entry->n_values; i++)
        {
            if (entry->values[i].value)
                free(entry->values[i].value);
            if (entry->values[i].label)
                free(entry->values[i].label);
        }
    }
    free(entry);
}

OptionListEntry *OptionListGetEntryByNumber(OptionList *list, int n)
{
    if (!list)
        return NULL;

    OptionListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

OptionListEntry *OptionListGetEntryByKey(OptionList *list, char *key)
{
    if (!list || !key)
        return NULL;

    OptionListEntry *entry = list->head;

    while (entry)
    {
        if (entry->key && strcmp(entry->key, key) == 0)
            return entry;

        entry = entry->next;
    }

    return NULL;
}

int OptionListRemoveEntry(OptionList *list, OptionListEntry *entry)
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

    OptionListFreeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

void OptionListAddEntry(OptionList *list, OptionListEntry *entry)
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
        OptionListEntry *tail = list->tail;
        tail->next = entry;
        entry->previous = tail;
        list->tail = entry;
    }

    list->length++;
}

void OptionListEmpty(OptionList *list)
{
    if (!list)
        return;

    OptionListEntry *entry = list->head;

    while (entry)
    {
        OptionListEntry *next = entry->next;
        OptionListFreeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

int OptionListSaveConfig(OptionList *list, const char *path)
{
    if (!list)
        return -1;

    SceUID fd = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
        return fd;

    int ret = 0;
    OptionListEntry *entry = list->head;

    while (entry)
    {
        if ((ret = sceIoWrite(fd, entry->key, strlen(entry->key))) < 0)
            break;

        if ((ret = sceIoWrite(fd, " = \"", 4)) < 0)
            break;

        if ((ret = sceIoWrite(fd, entry->values[entry->sel_pos].value, strlen(entry->values[entry->sel_pos].value))) < 0)
            break;

        if ((ret = sceIoWrite(fd, "\"\n", 2)) < 0)
            break;

        entry = entry->next;
    }

    sceIoClose(fd);
    if (ret < 0)
        sceIoRemove(path);
    return ret;
}
