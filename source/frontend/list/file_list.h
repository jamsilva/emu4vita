#ifndef __M_FILE_LIST_H__
#define __M_FILE_LIST_H__

#include "linked_list.h"
#include "file.h"

enum FileSortFlags
{
    SORT_NONE,
    SORT_BY_NAME,
    SORT_BY_SIZE,
    SORT_BY_DATE,
};

typedef struct
{
    char *name;
    int name_length;
    int is_folder;
    int type;
} FileListEntryData;

typedef struct
{
    char path[MAX_PATH_LENGTH];
    int files;
    int folders;
} FileListData;

#define HOME_PATH "home"

int getNumberOfDevices();
char **getDevices();

LinkedListEntry *FileListFindEntryByName(LinkedList *list, const char *name);
int FileListGetNumberByName(LinkedList *list, const char *name);

int FileListGetDeviceEntries(LinkedList *list);
int FileListGetDirectoryEntries(LinkedList *list, const char *path, int sort);
int FileListGetEntries(LinkedList *list, const char *path, int sort);
LinkedList *NewFileList();

#endif