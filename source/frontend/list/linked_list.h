#ifndef __M_LINKED_LIST_H__
#define __M_LINKED_LIST_H__

typedef void (*LinkedListFreeDataCallback)(void *data);
typedef int (*LinkedListCompareCallback)(void *data1, void *data2);

typedef struct LinkedListEntry LinkedListEntry;
typedef struct LinkedList LinkedList;

void *LinkedListGetEntryData(LinkedListEntry *entry);
void LinkedListSetEntryData(LinkedListEntry *entry, void *data);
void *LinkedListGetListData(LinkedList *list);
void LinkedListSetListData(LinkedList *list, void *data);
int LinkedListGetLength(LinkedList *list);
void LinkedListSetFreeEntryDataCallback(LinkedList *list, LinkedListFreeDataCallback callback);
void LinkedListSetFreeListDataCallback(LinkedList *list, LinkedListFreeDataCallback callback);
void LinkedListSetCompareCallback(LinkedList *list, LinkedListCompareCallback callback);

void LinkedListFreeEntryData(LinkedList *list, void *data);
void LinkedListFreeListData(LinkedList *list, void *data);

LinkedListEntry *LinkedListHead(LinkedList *list);
LinkedListEntry *LinkedListTail(LinkedList *list);
LinkedListEntry *LinkedListNext(LinkedListEntry *entry);
LinkedListEntry *LinkedListPrev(LinkedListEntry *entry);
LinkedListEntry *LinkedListFind(LinkedList *list, int n);
int LinkedListRemove(LinkedList *list, LinkedListEntry *entry);
LinkedListEntry *LinkedListAdd(LinkedList *list, void *data);
void LinkedListEmpty(LinkedList *list);
LinkedList *LinkedListCreat();
void LinkedListDestroy(LinkedList *list);

#endif