#ifndef __M_STRING_LIST_H__
#define __M_STRING_LIST_H__

#include "linked_list.h"

LinkedListEntry *StringListAdd(LinkedList *list, const char *text);
LinkedList *StringListCreate();

#endif
