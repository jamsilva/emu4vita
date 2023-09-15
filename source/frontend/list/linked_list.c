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
    LinkedListFreeDataCallback freeEntryData;
};

struct LinkedList
{
    LinkedListEntry *head;
    LinkedListEntry *tail;
    int length;
    void *data;
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

void LinkedListEntrySetFreeEntryDataCallback(LinkedListEntry *entry, LinkedListFreeDataCallback callback)
{
    if (entry)
        entry->freeEntryData = callback;
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

LinkedListEntry *LinkedListFindByNum(LinkedList *list, int n)
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

LinkedListEntry *LinkedListFindByData(LinkedList *list, void *data)
{
    if (!list)
        return NULL;

    LinkedListEntry *entry = list->head;

    while (entry)
    {
        if (entry->data == data)
            return entry;
        entry = entry->next;
    }

    return NULL;
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

int LinkedListAddEx(LinkedList *list, void *data, LinkedListFreeDataCallback freeEntryData)
{
    if (!list || !data)
        return 0;

    LinkedListEntry *entry = malloc(sizeof(LinkedListEntry));
    if (!entry)
        return 0;

    entry->next = NULL;
    entry->prev = NULL;
    entry->data = data;
    entry->freeEntryData = freeEntryData;

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
                if (list->compare(data, insert->data) < 0)
                    break;
                insert = insert->next;
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

    return 1;
}

int LinkedListAdd(LinkedList *list, void *data)
{
    return LinkedListAddEx(list, data, NULL);
}

void LinkedListEmpty(LinkedList *list)
{
    if (!list)
        return;

    LinkedListEntry *entry = list->head;

    while (entry)
    {
        LinkedListEntry *next = entry->next;
        if (entry->data)
        {
            if (entry->freeEntryData)
                entry->freeEntryData(entry->data);
            else if (list->freeEntryData)
                list->freeEntryData(entry->data);
        }
        free(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

void LinkedListDestroy(LinkedList *list)
{
    if (!list)
        return;

    LinkedListEmpty(list);

    if (list->data && list->freeListData)
        list->freeListData(list->data);
    free(list);
}

LinkedList *NewLinkedList()
{
    LinkedList *list = (LinkedList *)calloc(1, sizeof(LinkedList));
    if (!list)
        return NULL;

    return list;
}
