#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/devctl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#include "file_list.h"
#include "config.h"
#include "init.h"
#include "utils.h"
#include "strnatcmp.h"

static char *devices[] = {
    "imc0:",
    "uma0:",
    "ur0:",
    "ux0:",
    "xmc0:",
};

#define N_DEVICES (sizeof(devices) / sizeof(char *))

int getNumberOfDevices()
{
    return N_DEVICES;
}

char **getDevices()
{
    return devices;
}

static void freeEntryData(void *data)
{
    FileListEntryData *e_data = (FileListEntryData *)data;
    if (e_data)
    {
        free(e_data->name);
        free(e_data);
    }
}

LinkedListEntry *FileListFindEntryByName(LinkedList *list, const char *name)
{
    if (!list || !name)
        return NULL;

    LinkedListEntry *entry = LinkedListHead(list);

    int name_length = strlen(name);

    while (entry)
    {
        FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
        if (data->name_length == name_length && strcasecmp(data->name, name) == 0)
            return entry;

        entry = LinkedListNext(entry);
    }

    return NULL;
}

int FileListGetNumberByName(LinkedList *list, const char *name)
{
    if (!list || !name)
        return -1;

    LinkedListEntry *entry = LinkedListHead(list);

    int name_length = strlen(name);

    int n = 0;

    while (entry)
    {
        FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
        if (data->name_length == name_length && strcasecmp(data->name, name) == 0)
            return n;

        n++;
        entry = LinkedListNext(entry);
    }

    return -1;
}

static int FileListCompareByName(void *data1, void *data2)
{
    if (!data1 || !data2)
        return -1;

    FileListEntryData *f_data1 = (FileListEntryData *)data1;
    FileListEntryData *f_data2 = (FileListEntryData *)data2;
    char name1[MAX_NAME_LENGTH];
    char name2[MAX_NAME_LENGTH];

    strcpy(name1, f_data1->name);
    RemoveEndSlash(name1);
    strcpy(name2, f_data2->name);
    RemoveEndSlash(name2);

    // Sort by name within the same type
    if (f_data1->is_folder == f_data2->is_folder)
        return strnatcasecmp(name1, name2);
    else
        return f_data1->is_folder ? -1 : 1;
}

int FileListGetDeviceEntries(LinkedList *list)
{
    if (!list)
        return -1;

    LinkedListSetCompareCallback(list, FileListCompareByName);

    FileListData *ls_data = (FileListData *)LinkedListGetListData(list);

    int i;
    for (i = 0; i < N_DEVICES; i++)
    {
        if (devices[i])
        {
            if (is_safe_mode && strcmp(devices[i], "ux0:") != 0)
                continue;

            SceIoStat stat;
            memset(&stat, 0, sizeof(SceIoStat));
            if (sceIoGetstat(devices[i], &stat) >= 0)
            {
                FileListEntryData *e_data = malloc(sizeof(FileListEntryData));
                if (e_data)
                {
                    e_data->name_length = strlen(devices[i]);
                    e_data->name = malloc(e_data->name_length + 1);
                    strcpy(e_data->name, devices[i]);
                    e_data->is_folder = 1;

                    LinkedListAdd(list, e_data);

                    ls_data->folders++;
                }
            }
        }
    }

    return 0;
}

int FileListGetDirectoryEntries(LinkedList *list, const char *path, int sort)
{
    if (!list)
        return -1;

    if (sort == SORT_BY_NAME)
        LinkedListSetCompareCallback(list, FileListCompareByName);
    else
        LinkedListSetCompareCallback(list, NULL);

    SceUID dfd = sceIoDopen(path);
    if (dfd < 0)
        return dfd;

    FileListData *ls_data = (FileListData *)LinkedListGetListData(list);
    int res = 0;

    do
    {
        SceIoDirent dir;
        memset(&dir, 0, sizeof(SceIoDirent));

        res = sceIoDread(dfd, &dir);
        if (res > 0)
        {
            int is_folder = SCE_S_ISDIR(dir.d_stat.st_mode);

            if (is_folder && strcasecmp(dir.d_name, PREVIEW_DIR_NAME) == 0)
                continue;

            if (!is_folder && !IsValidFile(dir.d_name))
                continue;

            FileListEntryData *e_data = malloc(sizeof(FileListEntryData));
            if (!e_data)
                continue;

            e_data->is_folder = is_folder;

            if (is_folder)
            {
                e_data->name_length = strlen(dir.d_name) + 1;
                e_data->name = malloc(e_data->name_length + 1);
                strcpy(e_data->name, dir.d_name);
                AddEndSlash(e_data->name);
                ls_data->folders++;
            }
            else
            {
                e_data->name_length = strlen(dir.d_name);
                e_data->name = malloc(e_data->name_length + 1);
                strcpy(e_data->name, dir.d_name);
                ls_data->files++;
            }

            LinkedListAdd(list, e_data);
        }
    } while (res > 0);

    sceIoDclose(dfd);

    return 0;
}

int FileListGetEntries(LinkedList *list, const char *path, int sort)
{
    if (!list)
        return -1;

    if (strcasecmp(path, HOME_PATH) == 0)
    {
        return FileListGetDeviceEntries(list);
    }

    return FileListGetDirectoryEntries(list, path, sort);
}

LinkedList *NewFileList()
{
    LinkedList *list = NewLinkedList();
    if (!list)
        return NULL;

    FileListData *data = (FileListData *)calloc(1, sizeof(FileListData));
    if (!data)
    {
        LinkedListDestroy(list);
        return NULL;
    }

    LinkedListSetListData(list, data);
    LinkedListSetFreeEntryDataCallback(list, freeEntryData);
    LinkedListSetFreeListDataCallback(list, free);

    return list;
}
