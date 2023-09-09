#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "string_list.h"

int StringListAdd(LinkedList *list, const char *text)
{
    if (!list || !text)
        return 0;
    
    char *string = malloc(strlen(text) + 1);
    strcpy(string, text);

    return LinkedListAdd(list, string);
}

LinkedList *StringListCreate()
{
    LinkedList *list = LinkedListCreat();
    if (!list)
        return NULL;

    LinkedListSetFreeEntryDataCallback(list, free);
    return list;
}
