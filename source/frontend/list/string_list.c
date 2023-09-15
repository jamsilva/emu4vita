#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "string_list.h"

int StringListAdd(LinkedList *list, const char *string)
{
    if (!list || !string)
        return 0;

    char *str = malloc(strlen(string) + 1);
    if (!str)
        return 0;
    strcpy(str, string);

    LinkedListAdd(list, str);

    return 1;
}

LinkedList *NewStringList()
{
    LinkedList *list = NewLinkedList();
    if (!list)
        return NULL;

    LinkedListSetFreeEntryDataCallback(list, free);
    return list;
}
