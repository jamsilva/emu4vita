#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "overlay_list.h"
#include "config_text.h"
#include "file.h"

enum TypeOverlayText
{
    TYPE_OVERLAY_TEXT_NONE,
    TYPE_OVERLAY_TEXT_ITEM,
    TYPE_OVERLAY_TEXT_NAME_VALUE,
};

void OverlayListFreeEntry(OverlayListEntry *entry)
{
    if (!entry)
        return;

    if (entry->name)
        free(entry->name);
    if (entry->image_name)
        free(entry->image_name);
    free(entry);
}

OverlayListEntry *OverlayListGetEntryByNumber(OverlayList *list, int n)
{
    if (!list)
        return NULL;

    OverlayListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

OverlayListEntry *OverlayListGetEntryByName(OverlayList *list, const char *name)
{
    if (!list)
        return NULL;

    OverlayListEntry *entry = list->head;

    int name_length = strlen(name);

    while (entry)
    {
        if (strlen(entry->name) == name_length && strcasecmp(entry->name, name) == 0)
            return entry;

        entry = entry->next;
    }

    return NULL;
}

int OverlayListGetNumberByName(OverlayList *list, const char *name)
{
    if (!list)
        return -1;

    OverlayListEntry *entry = list->head;

    int name_length = strlen(name);

    int n = 0;

    while (entry)
    {
        if (strlen(entry->name) == name_length && strcasecmp(entry->name, name) == 0)
            return n;

        n++;
        entry = entry->next;
    }

    return -2;
}

int OverlayListRemoveEntry(OverlayList *list, OverlayListEntry *entry)
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

    OverlayListFreeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

void OverlayListAddEntry(OverlayList *list, OverlayListEntry *entry)
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
        OverlayListEntry *tail = list->tail;
        tail->next = entry;
        entry->previous = tail;
        list->tail = entry;
    }

    list->length++;
}

void OverlayListEmpty(OverlayList *list)
{
    if (!list)
        return;

    OverlayListEntry *entry = list->head;

    while (entry)
    {
        OverlayListEntry *next = entry->next;
        OverlayListFreeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

static char *getItem(const char *str)
{
    if (str[0] != '[')
        return NULL;

    char *p = strchr(str + 1, ']');
    if (!p)
        return NULL;

    int len = p - (str + 1);

    char *out = malloc(len + 1);
    if (out)
    {
        strncpy(out, str + 1, len);
        out[len] = '\0';
    }

    return out;
}

static int readLine(char *line, char **pkey, char **pvalue)
{
    if (!line)
        return TYPE_OVERLAY_TEXT_NONE;

    int res = TYPE_OVERLAY_TEXT_NONE;
    char *key = NULL;
    char *value = NULL;
    const char *head = line;
    const char *tail = line + strlen(line);
    int len;

    // Trim at beginning
    while (*head == ' ' || *head == '\t')
        head++;

    // Ignore comments #1
    if (head[0] == '#')
    {
        // AppLog("IGNORE %s\n", line);
        goto END;
    }

    // Ignore comments #2
    const char *p = strchr(head, '#');
    if (p)
    {
        // AppLog("IGNORE %s\n", p);
        tail = p;
    }

    // Get item
    if (line[0] == '[')
    {
        key = getItem(line);
        if (key != NULL)
            res = TYPE_OVERLAY_TEXT_ITEM;
        goto END;
    }

    // Get token
    p = strchr(head, '=');
    if (!p || p >= tail)
        goto END;

    // Key
    len = p - head;
    key = malloc(len + 1);
    if (!key)
        goto END;
    strncpy(key, head, len);
    key[len] = '\0';
    TrimString(key);
    // AppLog("KEY: %s\n", key);

    head = p + 1;
    while (*head == ' ' || *head == '\t')
        head++;

    if (head[0] == '"') // String
    {
        head++;
        p = strchr(head, '"');
        if (!p || p >= tail)
            goto END;
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
        goto END;
    strncpy(value, head, len);
    value[len] = '\0';
    // AppLog("VALUE: %s\n", value);

    res = TYPE_OVERLAY_TEXT_NAME_VALUE;

END:
    if (res == TYPE_OVERLAY_TEXT_NONE)
    {
        if (key)
            free(key);
        if (value)
            free(value);
    }
    else
    {
        *pkey = key;
        *pvalue = value;
    }
    return res;
}

int OverlayListGetEntriesFromBuffer(OverlayList *list, void *buffer, int size)
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

    OverlayListEntry *entry = NULL;
    char *line = NULL;
    char *name = NULL, *value = NULL;
    int type;

    do
    {
        res = ConfigGetLine(p, size, &line);
        // AppLog("ConfigGetLine: line = %s\n", line);
        if (res > 0)
        {
            type = readLine(line, &name, &value);

            if (type == TYPE_OVERLAY_TEXT_ITEM)
            {
                if (name)
                {
                    entry = (OverlayListEntry *)calloc(1, sizeof(OverlayListEntry));
                    if (entry)
                    {
                        entry->name = name;
                        name = NULL;
                        OverlayListAddEntry(list, entry);
                    }
                }
            }
            else if (type == TYPE_OVERLAY_TEXT_NAME_VALUE)
            {
                if (entry && name && value)
                {
                    if (strcasecmp(name, "image_name") == 0)
                    {
                        if (!entry->image_name)
                        {
                            entry->image_name = value;
                            value = NULL;
                        }
                    }
                    else if (strcasecmp(name, "viewport_rotate") == 0)
                    {
                        if (!entry->viewport_rotate)
                        {
                            entry->viewport_rotate = (int *)malloc(sizeof(int));
                            if (entry->viewport_rotate)
                                *(entry->viewport_rotate) = StringToDecimal(value);
                        }
                    }
                    else if (strcasecmp(name, "viewport_width") == 0)
                    {
                        if (!entry->viewport_width)
                        {
                            entry->viewport_width = (int *)malloc(sizeof(int));
                            if (entry->viewport_width)
                                *(entry->viewport_width) = StringToDecimal(value);
                        }
                    }
                    else if (strcasecmp(name, "viewport_height") == 0)
                    {
                        if (!entry->viewport_height)
                        {
                            entry->viewport_height = (int *)malloc(sizeof(int));
                            if (entry->viewport_height)
                                *(entry->viewport_height) = StringToDecimal(value);
                        }
                    }
                    else if (strcasecmp(name, "viewport_x") == 0)
                    {
                        if (!entry->viewport_x)
                        {
                            entry->viewport_x = (int *)malloc(sizeof(int));
                            if (entry->viewport_x)
                                *(entry->viewport_x) = StringToDecimal(value);
                        }
                    }
                    else if (strcasecmp(name, "viewport_y") == 0)
                    {
                        if (!entry->viewport_y)
                        {
                            entry->viewport_y = (int *)malloc(sizeof(int));
                            if (entry->viewport_y)
                                *(entry->viewport_y) = StringToDecimal(value);
                        }
                    }
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

int OverlayListGetEntries(OverlayList *list, const char *path)
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
