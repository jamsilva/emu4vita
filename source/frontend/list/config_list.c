#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "config_list.h"
#include "linked_list.h"
#include "utils_string.h"
#include "file.h"

static void freeEntryData(void *data)
{
    ConfigListEntryData *e_data = (ConfigListEntryData *)data;
    if (e_data)
    {
        if (e_data->key)
            free(e_data->key);
        if (e_data->value)
            free(e_data->value);
        free(e_data);
    }
}

LinkedListEntry *ConfigListFindEntryByKey(LinkedList *list, const char *key)
{
    if (!list || !key)
        return NULL;

    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        ConfigListEntryData *data = (ConfigListEntryData *)LinkedListGetEntryData(entry);

        if (data->key && strcmp(data->key, key) == 0)
            return entry;

        entry = LinkedListNext(entry);
    }

    return NULL;
}

int ConfigListGetEntriesFromBuffer(LinkedList *list, void *buffer, int size)
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

    ConfigListEntryData *data = NULL;
    char *line = NULL;
    char *name = NULL, *value = NULL;

    do
    {
        res = StringGetLine(p, size, &line);
        // printf("StringGetLine: line = %s\n", line);
        if (res > 0)
        {
            if (StringReadConfigLine(line, &name, &value) >= 0)
            {
                // printf("StringGetLine: name = %s, value = %s\n", name, value);
                data = (ConfigListEntryData *)calloc(1, sizeof(ConfigListEntryData));
                if (data)
                {
                    LinkedListAdd(list, data);
                    data->key = name;
                    data->value = value;
                    name = NULL;
                    value = NULL;
                }
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

int ConfigListGetEntries(LinkedList *list, const char *path)
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

LinkedList *NewConfigList()
{
    LinkedList *list = NewLinkedList();
    if (!list)
        return NULL;

    LinkedListSetFreeEntryDataCallback(list, freeEntryData);
    return list;
}
