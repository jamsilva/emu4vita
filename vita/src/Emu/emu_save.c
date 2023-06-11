#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "Activity/browser.h"
#include "Retro/retro.h"
#include "file.h"
#include "utils.h"
#include "config.h"

static int makeSavefilePath(char *path, int id)
{
    char name[MAX_NAME_LENGTH];
    MakeCurrentFileName(name);
    char base_name[MAX_NAME_LENGTH];
    MakeBaseName(base_name, name, MAX_NAME_LENGTH);
    if (id == RETRO_MEMORY_SAVE_RAM)
        snprintf(path, MAX_PATH_LENGTH, "%s/%s.srm", (CORE_SAVEFILES_DIR), base_name);
    else if (id == RETRO_MEMORY_RTC)
        snprintf(path, MAX_PATH_LENGTH, "%s/%s.rtc", (CORE_SAVEFILES_DIR), base_name);
    else
        snprintf(path, MAX_PATH_LENGTH, "%s/%s.unk", (CORE_SAVEFILES_DIR), base_name);
    return 0;
}

static int loadMemoryFile(int id)
{
    size_t dst_size = retro_get_memory_size(id);
    if (dst_size == 0)
        return -1;

    void *dst_data = retro_get_memory_data(id);
    if (dst_data == NULL)
        return -1;

    char path[MAX_PATH_LENGTH];
    makeSavefilePath(path, id);
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int64_t src_size = sceIoLseek(fd, 0, SCE_SEEK_END);
    if (src_size <= 0)
    {
        AppLog("Emu_LoadSrm failed: get file size failed\n");
        sceIoClose(fd);
        return -1;
    }
    if (src_size > dst_size)
    {
        AppLog("Emu_LoadSrm failed: SRAM is larger than implementation expects\n");
        sceIoClose(fd);
        return -1;
    }

    void *src_data = malloc(src_size);
    if (src_data == NULL)
    {
        sceIoClose(fd);
        return -1;
    }

    sceIoLseek(fd, 0, SCE_SEEK_SET);
    char *buf = (char *)src_data;
    int64_t remain = src_size;
    int64_t transfer = TRANSFER_SIZE;

    while (remain > 0)
    {
        if (remain < TRANSFER_SIZE)
            transfer = remain;
        else
            transfer = TRANSFER_SIZE;

        int read = sceIoRead(fd, buf, transfer);
        if (read < 0)
        {
            free(src_data);
            sceIoClose(fd);
            return -1;
        }
        if (read == 0)
            break;

        buf += read;
        remain -= read;
    }
    sceIoClose(fd);

    memcpy(dst_data, src_data, src_size);
    free(src_data);

    return 0;
}

static int saveMemoryFile(int id)
{
    size_t size = retro_get_memory_size(id);
    if (size == 0)
        return -1;

    void *data = retro_get_memory_data(id);
    if (data == NULL)
        return -1;

    char path[MAX_PATH_LENGTH];
    makeSavefilePath(path, id);
    CreateFolder(CORE_SAVEFILES_DIR);
    SceUID fd = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
        return fd;

    char *buf = (char *)data;
    int64_t remain = size;
    int64_t transfer = TRANSFER_SIZE;

    while (remain > 0)
    {
        if (remain < TRANSFER_SIZE)
            transfer = remain;
        else
            transfer = TRANSFER_SIZE;

        int written = sceIoWrite(fd, buf, transfer);
        if (written < 0)
        {
            sceIoClose(fd);
            return -1;
        }

        buf += written;
        remain -= written;
    }
    sceIoClose(fd);

    return 0;
}

int Emu_LoadSrm()
{
    loadMemoryFile(RETRO_MEMORY_SAVE_RAM);
    loadMemoryFile(RETRO_MEMORY_RTC);
    return 0;
}

int Emu_SaveSrm()
{
    saveMemoryFile(RETRO_MEMORY_SAVE_RAM);
    saveMemoryFile(RETRO_MEMORY_RTC);
    return 0;
}

int Emu_DeleteSrm()
{
    char path[MAX_PATH_LENGTH];
    makeSavefilePath(path, RETRO_MEMORY_SAVE_RAM);
    sceIoRemove(path);
    makeSavefilePath(path, RETRO_MEMORY_RTC);
    sceIoRemove(path);
    return 0;
}