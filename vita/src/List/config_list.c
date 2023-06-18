#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "config_list.h"
#include "config_lib.h"
#include "file.h"

void ConfigListFreeEntry(ConfigListEntry *entry)
{
    if (!entry)
        return;

    if (entry->key)
        free(entry->key);
    if (entry->value)
        free(entry->value);
    free(entry);
}

ConfigListEntry *ConfigListGetEntryByNumber(ConfigList *list, int n)
{
    if (!list)
        return NULL;

    ConfigListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

ConfigListEntry *ConfigListGetEntryByKey(ConfigList *list, char *key)
{
    if (!list || !key)
        return NULL;

    ConfigListEntry *entry = list->head;

    while (entry)
    {
        if (entry->key && strcmp(entry->key, key) == 0)
            return entry;

        entry = entry->next;
    }

    return NULL;
}

int ConfigListRemoveEntry(ConfigList *list, ConfigListEntry *entry)
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

    ConfigListFreeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

void ConfigListAddEntry(ConfigList *list, ConfigListEntry *entry)
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
        ConfigListEntry *tail = list->tail;
        tail->next = entry;
        entry->previous = tail;
        list->tail = entry;
    }

    list->length++;
}

void ConfigListAddEntryEx(ConfigList *list, char *key, char *value, int copy)
{
    if (!list)
        return;

    ConfigListEntry *entry = (ConfigListEntry *)malloc(sizeof(ConfigListEntry));
    memset(entry, 0, sizeof(ConfigListEntry));

    if (!copy)
    {
        entry->key = key;
        entry->value = value;
    }
    else
    {
        if (key)
        {
            entry->key = (char *)malloc(strlen(key) + 1);
            if (entry->key)
                strcpy(entry->key, key);
        }
        if (value)
        {
            entry->value = (char *)malloc(strlen(value) + 1);
            if (entry->value)
                strcpy(entry->value, value);
        }
    }

    ConfigListAddEntry(list, entry);
}

void ConfigListEmpty(ConfigList *list)
{
    if (!list)
        return;

    ConfigListEntry *entry = list->head;

    while (entry)
    {
        ConfigListEntry *next = entry->next;
        ConfigListFreeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

int ConfigListGetEntriesFromBuffer(ConfigList *list, void *buffer, int size)
{
    if (!list)
        return -1;

    int res = 0;
    char *p = (char *)buffer;

    // Skip UTF-8 bom
    uint32_t bom = 0xBFBBEF;
    if (memcmp(p, &bom, 3) == 0)
    {
        p += 3;
        size -= 3;
    }

    ConfigListEntry *entry = NULL;
    char *line = NULL;
    char *name = NULL, *value = NULL;

    do
    {
        res = ConfigGetLine(p, size, &line);
        // printf("ConfigGetLine: line = %s\n", line);
        if (res > 0)
        {
            if (ConfigReadLine(line, &name, &value) >= 0)
            {
                // printf("ConfigGetLine: name = %s, value = %s\n", name, value);
                entry = (ConfigListEntry *)calloc(1, sizeof(ConfigListEntry));
                ConfigListAddEntry(list, entry);
                entry->key = name;
                entry->value = value;
                name = NULL;
                value = NULL;
            }

            if (line)
                free(line);
            line = NULL;
            if (name)
                free(name);
            name = NULL;
            if (value)
                free(value);
            value = NULL;

            size -= res;
            p += res;
        }
    } while (res > 0);

    return 0;
}

int ConfigListGetEntries(ConfigList *list, const char *path)
{
    if (!list)
        return -1;

    void *buffer = NULL;
    int size = AllocateReadFile(path, &buffer);
    if (size < 0)
        return size;

    ConfigListGetEntriesFromBuffer(list, buffer, size);

    free(buffer);

    return 0;
}