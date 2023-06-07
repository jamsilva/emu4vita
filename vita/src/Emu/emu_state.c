#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "Activity/browser.h"
#include "Setting/setting.h"
#include "Retro/retro.h"
#include "Emu/emu.h"
#include "Gui/gui.h"
#include "utils.h"
#include "file.h"
#include "config.h"

void MakeSaveStateDir(char *path)
{
    char name[MAX_NAME_LENGTH];
    MakeCurrentFileName(name);

    char base_name[MAX_NAME_LENGTH];
    MakeBaseName(base_name, name, MAX_NAME_LENGTH);
    snprintf(path, MAX_PATH_LENGTH, "%s/%s", (CORE_SAVESTATES_DIR), base_name);
}

void MakeSaveStatePath(char *path, int num)
{
    char dir[MAX_PATH_LENGTH];
    MakeSaveStateDir(dir);

    if (num == -1)
        snprintf(path, MAX_PATH_LENGTH, "%s/state-auto.bin", dir);
    else
        snprintf(path, MAX_PATH_LENGTH, "%s/state-%02d.bin", dir, num);
}

GUI_Texture *Emu_GetStateScreenshotTexture(int num)
{
    char path[MAX_PATH_LENGTH];
    GUI_Texture *tex = NULL;

    MakeSaveStatePath(path, num);
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
        return NULL;

    EmuStateHeader header;
    memset(&header, 0, sizeof(EmuStateHeader));
    sceIoRead(fd, &header, sizeof(EmuStateHeader));
    if (header.version != STATES_VERSION)
    {
        sceIoClose(fd);
        return false;
    }
    tex = GUI_CreatTextureFormat(header.screenshot_width, header.screenshot_height, SCE_GXM_TEXTURE_FORMAT_A8B8G8R8);
    if (tex)
    {
        sceIoLseek(fd, header.screenshot_offset, SCE_SEEK_SET);
        sceIoRead(fd, vita2d_texture_get_datap(tex), header.screenshot_size);
    }
    sceIoClose(fd);

    return tex;
}

GUI_Texture *Emu_GetStatePreviewTexture(int num)
{
    char path[MAX_PATH_LENGTH];
    GUI_Texture *tex = NULL;

    MakeSaveStatePath(path, num);
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
        return NULL;

    EmuStateHeader header;
    memset(&header, 0, sizeof(EmuStateHeader));
    sceIoRead(fd, &header, sizeof(EmuStateHeader));
    if (header.version != STATES_VERSION)
    {
        sceIoClose(fd);
        return false;
    }
    tex = GUI_CreatTextureFormat(header.preview_width, header.preview_height, SCE_GXM_TEXTURE_FORMAT_A8B8G8R8);
    if (tex)
    {
        sceIoLseek(fd, header.preview_offset, SCE_SEEK_SET);
        sceIoRead(fd, vita2d_texture_get_datap(tex), header.preview_size);
    }
    sceIoClose(fd);

    return tex;
}

int Emu_LoadState(int num)
{
    int ret = 0;
    void *state_buf = NULL;
    int fd = -1;
    char path[MAX_PATH_LENGTH];

    if (!Emu_IsGameLoaded())
        return -1;

    MakeSaveStatePath(path, num);

    fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
    {
        ret = -1;
        goto END;
    }

    EmuStateHeader header;
    memset(&header, 0, sizeof(EmuStateHeader));

    // Read header
    sceIoLseek(fd, 0, SCE_SEEK_SET);
    sceIoRead(fd, &header, sizeof(EmuStateHeader));

    if (header.version != STATES_VERSION)
    {
        AppLog("Emu_LoadState failed: state_version is different from app_version\n");
        ret = -1;
        goto END;
    }

    size_t serialize_size = retro_serialize_size();
    if (header.state_size != serialize_size)
    {
        AppLog("Emu_LoadState failed: state_size is different from serialize_size\n");
        ret = -1;
        goto END;
    }

    state_buf = malloc(header.state_size);
    if (!state_buf)
    {
        AppLog("Emu_LoadState failed: can't alloc state_buf\n");
        ret = -1;
        goto END;
    }

    char *buf = (char *)state_buf;
    int64_t remain = header.state_size;
    int64_t transfer = TRANSFER_SIZE;
    sceIoLseek(fd, header.state_offset, SCE_SEEK_SET);

    while (remain > 0)
    {
        if (remain < TRANSFER_SIZE)
            transfer = remain;
        else
            transfer = TRANSFER_SIZE;

        int read = sceIoRead(fd, buf, transfer);
        if (read < 0)
        {
            AppLog("Emu_LoadState failed: read file failed\n");
            ret = -1;
            goto END;
        }
        if (read == 0)
            break;

        buf += read;
        remain -= read;
    }

    if (!retro_unserialize(state_buf, header.state_size))
    {
        AppLog("Emu_LoadState failed: retro_unserialize failed\n");
        ret = -1;
        goto END;
    }

    Emu_CleanAudioSound();

END:
    if (state_buf)
        free(state_buf);
    if (fd >= 0)
        sceIoClose(fd);

    return ret;
}

int Emu_SaveState(int num)
{
    int ret = 0;
    uint32_t *screenshot_buf = NULL;
    uint32_t *preview_buf = NULL;
    void *state_buf = NULL;
    SceUID fd = -1;
    uint32_t screenshot_width;
    uint32_t screenshot_height;
    uint64_t screenshot_size;
    uint64_t screenshot_offset;
    uint32_t preview_width;
    uint32_t preview_height;
    uint64_t preview_size;
    uint64_t preview_offset;
    uint64_t state_size;
    uint64_t state_offset;
    char path[MAX_PATH_LENGTH];

    if (!Emu_IsGameLoaded())
        return -1;

    MakeSaveStateDir(path);
    CreateFolder(path);
    MakeSaveStatePath(path, num);

    uint32_t base_width, base_height;
    Emu_MakeVideoBaseWH(&base_width, &base_height);

    if (graphics_config.display_rotate == 1 || graphics_config.display_rotate == 3)
    {
        screenshot_width = base_height;
        screenshot_height = base_width;
    }
    else
    {
        screenshot_width = base_width;
        screenshot_height = base_height;
    }

    screenshot_size = 0;
    screenshot_offset = sizeof(EmuStateHeader);
    screenshot_buf = Emu_GetVideoScreenshotData(&screenshot_width, &screenshot_height, &screenshot_size, graphics_config.display_rotate);
    if (!screenshot_buf)
    {
        AppLog("Emu_SaveState failed: make screenshot failed!\n");
        ret = -1;
        goto END;
    }

    preview_offset = screenshot_offset + screenshot_size;
    preview_width = Setting_GetStatePreviewWidth();
    preview_height = Setting_GetStatePreviewHeight();
    preview_size = 0;
    preview_buf = Emu_GetVideoScreenshotData(&preview_width, &preview_height, &preview_size, graphics_config.display_rotate);
    if (!preview_buf)
    {
        AppLog("Emu_SaveState failed: make preview failed!\n");
        ret = -1;
        goto END;
    }

    state_offset = preview_offset + preview_size;
    state_size = retro_serialize_size();
    if (state_size == 0)
    {
        AppLog("Emu_SaveState failed: retro_serialize_size failed!\n");
        ret = -1;
        goto END;
    }
    state_buf = malloc(state_size);
    if (!state_buf)
    {
        AppLog("Emu_SaveState failed: can't alloc state_buf!\n");
        ret = -1;
        goto END;
    }
    retro_serialize(state_buf, state_size);

    fd = sceIoOpen(path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
    if (fd < 0)
    {
        ret = -1;
        goto END;
    }

    EmuStateHeader header;
    memset(&header, 0, sizeof(EmuStateHeader));
    header.version = STATES_VERSION;
    header.screenshot_offset = screenshot_offset;
    header.screenshot_width = screenshot_width;
    header.screenshot_height = screenshot_height;
    header.screenshot_size = screenshot_size;
    header.preview_offset = preview_offset;
    header.preview_width = preview_width;
    header.preview_height = preview_height;
    header.preview_size = preview_size;
    header.state_offset = state_offset;
    header.state_size = state_size;

    sceIoLseek(fd, 0, SCE_SEEK_SET);
    sceIoWrite(fd, &header, sizeof(EmuStateHeader));

    sceIoLseek(fd, screenshot_offset, SCE_SEEK_SET);
    sceIoWrite(fd, screenshot_buf, screenshot_size);

    sceIoLseek(fd, preview_offset, SCE_SEEK_SET);
    sceIoWrite(fd, preview_buf, preview_size);

    sceIoLseek(fd, state_offset, SCE_SEEK_SET);
    char *buf = (char *)state_buf;
    int64_t remain = state_size;
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
            AppLog("Emu_SaveState failed: write file failed\n");
            ret = -1;
            goto END;
        }

        buf += written;
        remain -= written;
    }

END:
    if (screenshot_buf)
        free(screenshot_buf);
    if (preview_buf)
        free(preview_buf);
    if (state_buf)
        free(state_buf);
    if (fd >= 0)
        sceIoClose(fd);

    return ret;
}

int Emu_DeleteState(int num)
{
    char path[MAX_PATH_LENGTH];
    MakeSaveStatePath(path, num);
    sceIoRemove(path);
    return 0;
}