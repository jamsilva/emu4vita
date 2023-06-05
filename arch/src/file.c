#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <psp2/io/devctl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <png.h>

#include "file.h"
#include "utils.h"

int ReadFile(const char *file, void *buf, int size)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int read = sceIoRead(fd, buf, size);

    sceIoClose(fd);
    return read;
}

int WriteFile(const char *file, const void *buf, int size)
{
    SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
        return fd;

    int written = sceIoWrite(fd, buf, size);

    sceIoClose(fd);
    return written;
}

int allocateReadFile(const char *file, void **buffer)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int size = sceIoLseek32(fd, 0, SCE_SEEK_END);
    sceIoLseek32(fd, 0, SCE_SEEK_SET);

    *buffer = malloc(size);
    if (!*buffer)
    {
        sceIoClose(fd);
        return -1;
    }

    int read = sceIoRead(fd, *buffer, size);
    sceIoClose(fd);

    return read;
}

int getFileSize(const char *file)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int fileSize = sceIoLseek(fd, 0, SCE_SEEK_END);

    sceIoClose(fd);
    return fileSize;
}

int checkFileExist(const char *file)
{
    SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
    if (fd < 0)
        return 0;

    sceIoClose(fd);
    return 1;
}

int checkFolderExist(const char *folder)
{
    SceUID dfd = sceIoDopen(folder);
    if (dfd < 0)
        return 0;

    sceIoDclose(dfd);
    return 1;
}

int createFolder(const char *path)
{
    if (checkFolderExist(path))
        return 0;

    int ret;
    int i, len;
    char str[MAX_PATH_LENGTH];
    strcpy(str, path);
    removeEndSlash(str);
    
    char *parent_path = getBaseDirectory(str);
    if (parent_path)
    {
        ret = createFolder(parent_path);
        free(parent_path);
        if (ret < 0)
            return ret;
    }

    ret = sceIoMkdir(str, 0777);
    if (ret < 0)
        return ret;

    return 0;
}

char *getBaseDirectory(const char *path)
{
    int i;
    int sep_ind = -1;
    int len = strlen(path);
    if (len > MAX_PATH_LENGTH - 1 || len <= 0)
        return NULL;
    for (i = len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            sep_ind = i;
            break;
        }
    }
    if (sep_ind == -1)
        return NULL;

    char *res = (char *)malloc(MAX_PATH_LENGTH);
    if (!res)
        return NULL;

    strncpy(res, path, MAX_PATH_LENGTH);
    res[sep_ind + 1] = '\0';
    return res;
}

char *getFilename(const char *path)
{
    int i;
    int sep_ind = -1;
    int len = strlen(path);
    if (len > MAX_PATH_LENGTH || len <= 0)
        return NULL;
    if (path[len - 1] == '/' || path[len - 1] == ':')
        return NULL; // no file

    for (i = len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            sep_ind = i;
            break;
        }
    }
    if (sep_ind == -1)
        return NULL;
    char *res = (char *)malloc(MAX_PATH_LENGTH);
    if (!res)
        return NULL;

    int new_len = len - (sep_ind + 1);
    strncpy(res, path + (sep_ind + 1), new_len); // dont copy separation char
    if (new_len + 1 < MAX_PATH_LENGTH)
        res[new_len] = '\0';
    else
        res[MAX_PATH_LENGTH - 1] = '\0';
    return res;
}

char *getBaseFilename(const char *path)
{
    int len = strlen(path);
    if (len > MAX_PATH_LENGTH || len <= 0)
        return NULL;
    if (path[len - 1] == '/' || path[len - 1] == ':')
        return NULL; // no file

    int sep_start = 0;
    int sep_end = len - 1;
    int i;
    for (i = len - 1; i >= 0; i--)
    {
        if (path[i] == '/' || path[i] == ':')
        {
            sep_start = i + 1;
            break;
        }
        else if ((path[i] == '.') && (sep_end == len - 1))
        {
            sep_end = i - 1;
        }
    }
    int new_len = sep_end - sep_start + 1;
    if (new_len <= 0)
        return NULL;
    char *res = (char *)malloc(MAX_PATH_LENGTH);
    if (!res)
        return NULL;
    strncpy(res, path + sep_start, new_len);
    if (new_len + 1 < MAX_PATH_LENGTH)
        res[new_len] = '\0';
    else
        res[MAX_PATH_LENGTH - 1] = '\0';
    return res;
}
