#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/kernel/threadmgr.h>

#include "list/overlay_list.h"
#include "setting.h"
#include "file.h"
#include "config.h"

static SceUID overlay_thid = -1;

static int OverlayThreadFunc(SceSize args, void *argp)
{
    int ret;
    char path[MAX_PATH_LENGTH];

    if (graphics_overlay_list)
        LinkedListDestroy(graphics_overlay_list);
    graphics_overlay_list = NewOverlayList();

    ret = -1;
    if (private_assets_dir)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s/%s", private_assets_dir, OVERLAYS_DIR_NAME, OVERLAYS_CONFIG_NAME);
        ret = OverlayListGetEntries(graphics_overlay_list, path);
    }
    if (ret < 0)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s/%s", public_assets_dir, OVERLAYS_DIR_NAME, OVERLAYS_CONFIG_NAME);
        ret = OverlayListGetEntries(graphics_overlay_list, path);
    }

    Setting_SetOverlayOption(graphics_overlay_list);
    sceKernelExitDeleteThread(0);
    return 0;
}

int Setting_InitOverlay()
{
    if (overlay_thid >= 0)
        Setting_WaitOverlayInitEnd();

    overlay_thid = sceKernelCreateThread("overlay_thread", OverlayThreadFunc, 0x10000100, 0x10000, 0, 0, NULL);
    if (overlay_thid >= 0)
        sceKernelStartThread(overlay_thid, 0, NULL);

    return overlay_thid;
}

int Setting_WaitOverlayInitEnd()
{
    if (overlay_thid >= 0)
    {
        sceKernelWaitThreadEnd(overlay_thid, NULL, NULL);
        sceKernelDeleteThread(overlay_thid);
        overlay_thid = -1;
    }

    return 0;
}
