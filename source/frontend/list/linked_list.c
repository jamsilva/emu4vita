#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

struct LinkedListEntry
{
    struct LinkedListEntry *next;
    struct LinkedListEntry *prev;
    void *data;
};

struct LinkedList
{
    LinkedListEntry *head;
    LinkedListEntry *tail;
    int length;
    void *data;
    int is_alloc;
    LinkedListFreeDataCallback freeEntryData;
    LinkedListFreeDataCallback freeListData;
    LinkedListCompareCallback compare;
};

void *LinkedListGetEntryData(LinkedListEntry *entry)
{
    return entry ? entry->data : NULL;
}

void LinkedListSetEntryData(LinkedListEntry *entry, void *data)
{
    if (entry)
        entry->data = data;
}

void *LinkedListGetListData(LinkedList *list)
{
    return list ? list->data : NULL;
}

void LinkedListSetListData(LinkedList *list, void *data)
{
    if (list)
        list->data = data;
}

int LinkedListGetLength(LinkedList *list)
{
    return list ? list->length : 0;
}

void LinkedListSetFreeEntryDataCallback(LinkedList *list, LinkedListFreeDataCallback callback)
{
    if (list)
        list->freeEntryData = callback;
}

void LinkedListSetFreeListDataCallback(LinkedList *list, LinkedListFreeDataCallback callback)
{
    if (list)
        list->freeListData = callback;
}

void LinkedListSetCompareCallback(LinkedList *list, LinkedListCompareCallback callback)
{
    if (list)
        list->compare = callback;
}

void LinkedListFreeEntryData(LinkedList *list, void *data)
{
    if (list && data && list->freeEntryData)
        list->freeEntryData(data);
}

void LinkedListFreeListData(LinkedList *list, void *data)
{
    if (list && data && list->freeListData)
        list->freeListData(data);
}

LinkedListEntry *LinkedListHead(LinkedList *list)
{
    return list ? list->head : NULL;
}

LinkedListEntry *LinkedListTail(LinkedList *list)
{
    return list ? list->tail : NULL;
}

LinkedListEntry *LinkedListNext(LinkedListEntry *entry)
{
    return entry ? entry->next : NULL;
}

LinkedListEntry *LinkedListPrev(LinkedListEntry *entry)
{
    return entry ? entry->prev : NULL;
}

LinkedListEntry *LinkedListFind(LinkedList *list, int n)
{
    if (!list)
        return NULL;

    LinkedListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

int LinkedListRemove(LinkedList *list, LinkedListEntry *entry)
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

    if (entry->data && list->freeEntryData)
        list->freeEntryData(entry->data);
    free(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

LinkedListEntry *LinkedListAdd(LinkedList *list, void *data)
{
    if (!list || !data)
        return NULL;

    LinkedListEntry *entry = malloc(sizeof(LinkedListEntry));
    if (!entry)
        return NULL;

    entry->next = NULL;
    entry->prev = NULL;
    entry->data = data;

    if (list->head == NULL)
    {
        list->head = entry;
        list->tail = entry;
    }
    else
    {
        LinkedListEntry *insert = NULL;

        if (list->compare)
        {
            insert = list->head;
            while (insert)
            {
                if (list->compare(data, insert->data) >= 0)
                    insert = insert->next;
                else
                    break;
            }
        }

        if (insert == NULL)
        {
            LinkedListEntry *tail = list->tail;
            tail->next = entry;
            entry->prev = tail;
            list->tail = entry;
        }
        else
        {
            if (insert->prev)
            {
                insert->prev->next = entry;
                entry->prev = insert->prev;
            }
            insert->prev = entry;
            entry->next = insert;
            if (insert == list->head)
                list->head = entry;
        }
    }

    list->length++;

    return entry;
}

void LinkedListEmpty(LinkedList *list)
{
    if (!list)
        return;

    LinkedListEntry *entry = list->head;

    while (entry)
    {
        LinkedListEntry *next = entry->next;
        if (entry->data && list->freeEntryData)
            list->freeEntryData(entry->data);
        free(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

LinkedList *LinkedListCreat()
{
    LinkedList *list = (LinkedList *)calloc(1, sizeof(LinkedList));
    if (!list)
        return NULL;

    list->is_alloc = 1;
    return list;
}

void LinkedListDestroy(LinkedList *list)
{
    if (!list)
        return;

    LinkedListEmpty(list);

    if (list->data && list->freeListData)
    {
        list->freeListData(list->data);
        list->data = NULL;
    }

    if (list->is_alloc)
        free(list);
}
