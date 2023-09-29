#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "option_list.h"
#include "linked_list.h"
#include "config_list.h"

static void freeEntryData(void *data)
{
    OptionListEntryData *e_data = (OptionListEntryData *)data;
    if (e_data)
    {
        if (e_data->key)
            free(e_data->key);
        if (e_data->desc)
            free(e_data->desc);
        if (e_data->values)
        {
            int i;
            for (i = 0; i < e_data->n_values; i++)
            {
                if (e_data->values[i].value)
                    free(e_data->values[i].value);
                if (e_data->values[i].label)
                    free(e_data->values[i].label);
            }
        }
        free(e_data);
    }
}

LinkedListEntry *OptionListFindEntryByKey(LinkedList *list, const char *key)
{
    if (!list || !key)
        return NULL;

    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);

        if (data->key && strcmp(data->key, key) == 0)
            return entry;

        entry = LinkedListNext(entry);
    }

    return NULL;
}

int OptionListResetConfig(LinkedList *list)
{
    if (!list)
        return -1;

    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);
        data->select = data->default_select;
        entry = LinkedListNext(entry);
    }

    return 0;
}

int OptionListLoadConfig(LinkedList *list, const char *path)
{
    if (!list)
        return -1;

    LinkedList *config_list = NewConfigList();
    if (!config_list)
        return -1;
    ConfigListGetEntries(config_list, path);

    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);
        data->select = data->default_select;

        if (data->key)
        {
            LinkedListEntry *find = ConfigListFindEntryByKey(config_list, data->key);
            if (find)
            {
                ConfigListEntryData *c_data = (ConfigListEntryData *)LinkedListGetEntryData(find);
                int i;
                for (i = 0; i < data->n_values; i++)
                {
                    if (data->values[i].value && strcmp(data->values[i].value, c_data->value) == 0)
                    {
                        data->select = i;
                        // printf("Option get config: key: %s, value: %s, select: %d\n", data->key, data->values[i].value, i);
                        break;
                    }
                }
            }
        }

        entry = LinkedListNext(entry);
    }

    LinkedListDestroy(config_list);

    return 0;
}

int OptionListSaveConfig(LinkedList *list, const char *path)
{
    if (!list)
        return -1;

    SceUID fd = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
        return fd;

    int ret = 0;
    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);

        if ((ret = sceIoWrite(fd, data->key, strlen(data->key))) < 0)
            break;

        if ((ret = sceIoWrite(fd, " = \"", 4)) < 0)
            break;

        if ((ret = sceIoWrite(fd, data->values[data->select].value, strlen(data->values[data->select].value))) < 0)
            break;

        if ((ret = sceIoWrite(fd, "\"\n", 2)) < 0)
            break;

        entry = LinkedListNext(entry);
    }

    sceIoClose(fd);
    if (ret < 0)
        sceIoRemove(path);
    return ret;
}

LinkedList *NewOptionList()
{
    LinkedList *list = NewLinkedList();
    if (!list)
        return NULL;

    LinkedListSetFreeEntryDataCallback(list, freeEntryData);
    return list;
}