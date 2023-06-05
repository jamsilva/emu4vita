#ifndef __M_FILE_LIST_H__
#define __M_FILE_LIST_H__

#include "file.h"

enum FileSortFlags
{
    SORT_NONE,
    SORT_BY_NAME,
    SORT_BY_SIZE,
    SORT_BY_DATE,
};

typedef struct FileListEntry
{
    struct FileListEntry *next;
    struct FileListEntry *previous;
    char *name;
    int name_length;
    int is_folder;
    int type;
} FileListEntry;

typedef struct
{
    FileListEntry *head;
    FileListEntry *tail;
    int length;
    char path[MAX_PATH_LENGTH];
    int files;
    int folders;
} FileList;

#define HOME_PATH "home"

int getNumberOfDevices();
char **getDevices();

void FileListFreeEntry(FileListEntry *entry);
FileListEntry *FileListCopyEntry(FileListEntry *src);

FileListEntry *FileListGetEntryByName(FileList *list, const char *name);
FileListEntry *FileListGetEntryByNumber(FileList *list, int n);
int FileListGetNumberByName(FileList *list, const char *name);

int FileListRemoveEntry(FileList *list, FileListEntry *entry);
int FileListRemoveEntryByName(FileList *list, const char *name);
void FileListAddEntry(FileList *list, FileListEntry *entry, int sort);
void FileListEmpty(FileList *list);

int FileListGetDeviceEntries(FileList *list);
int FileListGetDirectoryEntries(FileList *list, const char *path, int sort);
int FileListGetEntries(FileList *list, const char *path, int sort);

#endif