#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "overlay_list.h"
#include "linked_list.h"
#include "utils_string.h"
#include "file.h"

static void freeEntryData(void *data)
{
    OverlayListEntryData *e_data = (OverlayListEntryData *)data;
    if (e_data)
    {
        if (e_data->name)
            free(e_data->name);
        if (e_data->image_name)
            free(e_data->image_name);
        if (e_data->viewport_rotate)
            free(e_data->viewport_rotate);
        if (e_data->viewport_width)
            free(e_data->viewport_width);
        if (e_data->viewport_height)
            free(e_data->viewport_height);
        if (e_data->viewport_x)
            free(e_data->viewport_x);
        if (e_data->viewport_y)
            free(e_data->viewport_y);
        free(e_data);
    }
}

LinkedListEntry *ConfigListFindEntryByName(LinkedList *list, const char *name)
{
    if (!list || !name)
        return NULL;

    LinkedListEntry *entry = LinkedListHead(list);

    while (entry)
    {
        OverlayListEntryData *data = (OverlayListEntryData *)LinkedListGetEntryData(entry);

        if (data->name && strcmp(data->name, name) == 0)
            return entry;

        entry = LinkedListNext(entry);
    }

    return NULL;
}

static char *getItem(const char *str)
{
    const char *p = str;
    if (*p++ != '[')
        return NULL;

    const char *p2 = strchr(p, ']');
    if (!p2)
        return NULL;

    int len = p2 - p;

    char *res = malloc(len + 1);
    if (res)
    {
        strncpy(res, p, len);
        res[len] = '\0';
        // printf("ITEM: %s\n", res);
    }

    return res;
}

static int readLine(char *line, char **pkey, char **pvalue)
{
    char *key = NULL;
    char *value = NULL;
    const char *head = line;
    const char *tail = line + strlen(line);
    int len;

    if (!line)
        goto FAILED;

    // Trim at beginning
    while (*head == ' ' || *head == '\t')
        head++;

    // Ignore comments #1
    if (head[0] == '#')
    {
        // printf("IGNORE %s\n", line);
        goto FAILED;
    }

    // Ignore comments #2
    const char *p = strchr(head, '#');
    if (p)
    {
        // printf("IGNORE %s\n", p);
        tail = p;
    }

    // Get item
    if (line[0] == '[')
    {
        key = getItem(line);
        if (key)
            goto END;
        else
            goto FAILED;
    }

    // Get token
    p = strchr(head, '=');
    if (!p || p >= tail)
        goto FAILED;

    // Key
    len = p - head;
    key = malloc(len + 1);
    if (!key)
        goto FAILED;
    strncpy(key, head, len);
    key[len] = '\0';
    TrimString(key);
    // printf("KEY: %s\n", key);

    head = p + 1;
    while (*head == ' ' || *head == '\t')
        head++;

    if (head[0] == '"') // String
    {
        head++;
        p = strchr(head, '"');
        if (!p || p >= tail)
            goto FAILED;
    }
    else // Decimal
    {
        p = tail;
        while (p > head && (*(p - 1) == ' ' || *(p - 1) == '\t'))
            p--;
    }

    // value
    len = p - head;
    value = malloc(len + 1);
    if (!value)
        goto FAILED;
    strncpy(value, head, len);
    value[len] = '\0';
    // printf("VALUE: %s\n", value);

END:
    *pkey = key;
    *pvalue = value;
    return 0;

FAILED:
    if (key)
        free(key);
    if (value)
        free(value);
    *pkey = NULL;
    *pvalue = NULL;
    return -1;
}

static void setEntryData(LinkedList *list, OverlayListEntryData **data, char *key, char *value)
{
    if (!data || !key)
        goto END;

    if (!value)
    {
        if (list)
        {
            *data = (OverlayListEntryData *)calloc(1, sizeof(OverlayListEntryData));
            if (*data)
            {
                LinkedListAdd(list, *data);
                (*data)->name = key;
                key = NULL;
            }
        }
    }
    else if (*data)
    {
        if (strcasecmp(key, "image_name") == 0)
        {
            if (!(*data)->image_name)
            {
                (*data)->image_name = value;
                value = NULL;
            }
        }
        else if (strcasecmp(key, "viewport_rotate") == 0)
        {
            if (!(*data)->viewport_rotate)
            {
                (*data)->viewport_rotate = (int *)malloc(sizeof(int));
                if ((*data)->viewport_rotate)
                    *((*data)->viewport_rotate) = StringToDecimal(value);
            }
        }
        else if (strcasecmp(key, "viewport_width") == 0)
        {
            if (!(*data)->viewport_width)
            {
                (*data)->viewport_width = (int *)malloc(sizeof(int));
                if ((*data)->viewport_width)
                    *((*data)->viewport_width) = StringToDecimal(value);
            }
        }
        else if (strcasecmp(key, "viewport_height") == 0)
        {
            if (!(*data)->viewport_height)
            {
                (*data)->viewport_height = (int *)malloc(sizeof(int));
                if ((*data)->viewport_height)
                    *((*data)->viewport_height) = StringToDecimal(value);
            }
        }
        else if (strcasecmp(key, "viewport_x") == 0)
        {
            if (!(*data)->viewport_x)
            {
                (*data)->viewport_x = (int *)malloc(sizeof(int));
                if ((*data)->viewport_x)
                    *((*data)->viewport_x) = StringToDecimal(value);
            }
        }
        else if (strcasecmp(key, "viewport_y") == 0)
        {
            if (!(*data)->viewport_y)
            {
                (*data)->viewport_y = (int *)malloc(sizeof(int));
                if ((*data)->viewport_y)
                    *((*data)->viewport_y) = StringToDecimal(value);
            }
        }
    }

END:
    if (key)
        free(key);
    if (value)
        free(value);
}

int OverlayListGetEntriesFromBuffer(LinkedList *list, void *buffer, int size)
{
    if (!list)
        return -1;

    int res = 0;
    char *p = buffer;

    // Skip UTF-8 bom
    uint32_t bom = 0xBFBBEF;
    if (memcmp(p, &bom, 3) == 0)
    {
        p += 3;
        size -= 3;
    }

    OverlayListEntryData *data = NULL;
    char *line = NULL;
    char *key = NULL;
    char *value = NULL;

    do
    {
        res = StringGetLine(p, size, &line);
        // AppLog("StringGetLine: line = %s\n", line);
        if (res > 0)
        {
            readLine(line, &key, &value);
            setEntryData(list, &data, key, value);
            free(line);
            size -= res;
            p += res;
        }
    } while (res > 0);

    return 0;
}

int OverlayListGetEntries(LinkedList *list, const char *path)
{
    if (!list)
        return -1;

    void *buffer = NULL;
    int size = AllocateReadFile(path, &buffer);
    if (size < 0)
        return size;

    OverlayListGetEntriesFromBuffer(list, buffer, size);

    free(buffer);

    return 0;
}

LinkedList *NewOverlayList()
{
    LinkedList *list = NewLinkedList();
    if (!list)
        return NULL;

    LinkedListSetFreeEntryDataCallback(list, freeEntryData);
    return list;
}
