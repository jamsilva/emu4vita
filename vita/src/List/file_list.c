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

void FileListFreeEntry(FileListEntry *entry)
{
    if (!entry)
        return;

    free(entry->name);
    free(entry);
}

FileListEntry *FileListCopyEntry(FileListEntry *src)
{
    FileListEntry *dst = malloc(sizeof(FileListEntry));
    if (!dst)
        return NULL;

    memcpy(dst, src, sizeof(FileListEntry));
    dst->name = malloc(src->name_length + 1);
    strcpy(dst->name, src->name);
    return dst;
}

FileListEntry *FileListGetEntryByName(FileList *list, const char *name)
{
    if (!list || !name)
        return NULL;

    FileListEntry *entry = list->head;

    int name_length = strlen(name);

    while (entry)
    {
        if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0)
            return entry;

        entry = entry->next;
    }

    return NULL;
}

FileListEntry *FileListGetEntryByNumber(FileList *list, int n)
{
    if (!list)
        return NULL;

    FileListEntry *entry = list->head;

    while (n > 0 && entry)
    {
        n--;
        entry = entry->next;
    }

    if (n != 0)
        return NULL;

    return entry;
}

int FileListGetNumberByName(FileList *list, const char *name)
{
    if (!list || !name)
        return -1;

    FileListEntry *entry = list->head;

    int name_length = strlen(name);

    int n = 0;

    while (entry)
    {
        if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0)
            return n;

        n++;
        entry = entry->next;
    }

    return -1;
}

int FileListRemoveEntry(FileList *list, FileListEntry *entry)
{
    if (!list || !entry)
        return 0;

    if (entry->previous)
    {
        entry->previous->next = entry->next;
    }
    else
    {
        list->head = entry->next;
    }

    if (entry->next)
    {
        entry->next->previous = entry->previous;
    }
    else
    {
        list->tail = entry->previous;
    }

    FileListFreeEntry(entry);

    list->length--;

    if (list->length == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 1;
}

int FileListRemoveEntryByName(FileList *list, const char *name)
{
    if (!list)
        return 0;

    FileListEntry *entry = list->head;
    FileListEntry *previous = NULL;

    int name_length = strlen(name);

    while (entry)
    {
        if (entry->name_length == name_length && strcasecmp(entry->name, name) == 0)
        {
            if (previous)
            {
                previous->next = entry->next;
            }
            else
            {
                list->head = entry->next;
            }

            if (list->tail == entry)
            {
                list->tail = previous;
            }

            FileListFreeEntry(entry);

            list->length--;

            if (list->length == 0)
            {
                list->head = NULL;
                list->tail = NULL;
            }

            return 1;
        }

        previous = entry;
        entry = entry->next;
    }

    return 0;
}

void FileListAddEntry(FileList *list, FileListEntry *entry, int sort)
{
    if (!list || !entry)
        return;

    entry->next = NULL;
    entry->previous = NULL;

    if (list->head == NULL)
    {
        list->head = entry;
        list->tail = entry;
    }
    else
    {
        if (sort != SORT_NONE)
        {
            FileListEntry *p = list->head;
            FileListEntry *previous = NULL;

            char entry_name[MAX_NAME_LENGTH];
            strcpy(entry_name, entry->name);
            RemoveEndSlash(entry_name);

            while (p)
            {
                char p_name[MAX_NAME_LENGTH];
                strcpy(p_name, p->name);
                RemoveEndSlash(p_name);

                // Sort by type
                if (sort == SORT_BY_NAME)
                {
                    // First folders then files
                    if (entry->is_folder > p->is_folder)
                        break;

                    // Sort by name within the same type
                    if (entry->is_folder == p->is_folder)
                    {
                        if (strnatcasecmp(entry_name, p_name) < 0)
                        {
                            break;
                        }
                    }
                }

                previous = p;
                p = p->next;
            }

            if (previous == NULL)
            { // Order: entry (new head) -> p (old head)
                entry->next = p;
                p->previous = entry;
                list->head = entry;
            }
            else if (previous->next == NULL)
            { // Order: p (old tail) -> entry (new tail)
                FileListEntry *tail = list->tail;
                tail->next = entry;
                entry->previous = tail;
                list->tail = entry;
            }
            else
            { // Order: previous -> entry -> p
                previous->next = entry;
                entry->previous = previous;
                entry->next = p;
                p->previous = entry;
            }
        }
        else
        {
            FileListEntry *tail = list->tail;
            tail->next = entry;
            entry->previous = tail;
            list->tail = entry;
        }
    }

    list->length++;
}

void FileListEmpty(FileList *list)
{
    if (!list)
        return;

    FileListEntry *entry = list->head;

    while (entry)
    {
        FileListEntry *next = entry->next;
        FileListFreeEntry(entry);
        entry = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->files = 0;
    list->folders = 0;
}

int FileListGetDeviceEntries(FileList *list)
{
    if (!list)
        return -1;

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
                FileListEntry *entry = malloc(sizeof(FileListEntry));
                if (entry)
                {
                    entry->name_length = strlen(devices[i]);
                    entry->name = malloc(entry->name_length + 1);
                    strcpy(entry->name, devices[i]);
                    entry->is_folder = 1;

                    FileListAddEntry(list, entry, SORT_BY_NAME);

                    list->folders++;
                }
            }
        }
    }

    return 0;
}

int FileListGetDirectoryEntries(FileList *list, const char *path, int sort)
{
    if (!list)
        return -1;

    SceUID dfd = sceIoDopen(path);
    if (dfd < 0)
        return dfd;

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

            FileListEntry *entry = malloc(sizeof(FileListEntry));
            if (!entry)
                continue;

            entry->is_folder = is_folder;

            if (is_folder)
            {
                entry->name_length = strlen(dir.d_name) + 1;
                entry->name = malloc(entry->name_length + 1);
                strcpy(entry->name, dir.d_name);
                AddEndSlash(entry->name);
                list->folders++;
            }
            else
            {
                entry->name_length = strlen(dir.d_name);
                entry->name = malloc(entry->name_length + 1);
                strcpy(entry->name, dir.d_name);
                list->files++;
            }

            FileListAddEntry(list, entry, sort);
        }
    } while (res > 0);

    sceIoDclose(dfd);

    return 0;
}

int FileListGetEntries(FileList *list, const char *path, int sort)
{
    if (!list)
        return -1;

    if (strcasecmp(path, HOME_PATH) == 0)
    {
        return FileListGetDeviceEntries(list);
    }

    return FileListGetDirectoryEntries(list, path, sort);
}
