#ifndef __M_CONFIG_LIST_H__
#define __M_CONFIG_LIST_H__

typedef struct ConfigListEntry
{
    struct ConfigListEntry *next;
    struct ConfigListEntry *previous;
    char *key;
    char *value;
} ConfigListEntry;

typedef struct
{
    ConfigListEntry *head;
    ConfigListEntry *tail;
    int length;
} ConfigList;

void ConfigListFreeEntry(ConfigListEntry *entry);

ConfigListEntry *ConfigListGetEntryByNumber(ConfigList *list, int n);
ConfigListEntry *ConfigListGetEntryByKey(ConfigList *list, char *key);

int ConfigListRemoveEntry(ConfigList *list, ConfigListEntry *entry);
void ConfigListAddEntry(ConfigList *list, ConfigListEntry *entry);
void ConfigListAddEntryEx(ConfigList *list, char *key, char *value, int copy);
void ConfigListEmpty(ConfigList *list);

int ConfigListGetEntriesFromBuffer(ConfigList *list, void *buffer, int size);
int ConfigListGetEntries(ConfigList *list, const char *path);

#endif