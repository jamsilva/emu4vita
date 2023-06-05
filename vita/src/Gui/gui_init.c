#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/kernel/threadmgr.h>

#include "gui.h"
#include "file.h"
#include "config.h"

extern GUI_Activity browser_activity;

static SceUID gui_images_thid = -1;

static int ImagesThreadFunc(SceSize args, void *argp)
{
    GUI_Texture *texture = NULL;
    char path[MAX_PATH_LENGTH];

    if (private_assets_dir)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", private_assets_dir, WALLPAPER_PNG_NAME);
        texture = GUI_LoadPNGFile(path);
    }
    if (!texture)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", public_assets_dir, WALLPAPER_PNG_NAME);
        texture = GUI_LoadPNGFile(path);
    }
    if (texture)
    {
        GUI_SetWallpaperTexture(texture);
    }

    texture = NULL;
    if (private_assets_dir)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", private_assets_dir, SPLASH_PNG_NAME);
        texture = GUI_LoadPNGFile(path);
    }
    if (!texture)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", public_assets_dir, SPLASH_PNG_NAME);
        texture = GUI_LoadPNGFile(path);
    }
    if (texture)
    {

        GUI_SetSplashTexture(texture);
    }

    sceKernelExitDeleteThread(0);
    return 0;
}

static void GUI_InitImages()
{
    gui_images_thid = sceKernelCreateThread("gui_images_thread", ImagesThreadFunc, 0x10000100, 0x10000, 0, 0, NULL);
    if (gui_images_thid >= 0)
        sceKernelStartThread(gui_images_thid, 0, NULL);
}

static void GUI_DeinitImages()
{
    if (gui_images_thid >= 0)
    {
        sceKernelWaitThreadEnd(gui_images_thid, NULL, NULL);
        sceKernelDeleteThread(gui_images_thid);
        gui_images_thid = -1;
    }
    GUI_SetWallpaperTexture(NULL);
    GUI_SetWallpaperTexture(NULL);
}

void GUI_Init()
{
    GUI_InitLib();
    GUI_InitImages();
    GUI_InitShaders();

    GUI_GetEnterButton();
    GUI_RefreshLayout();
    GUI_SetMainActivity(&browser_activity);
    GUI_EnterActivity(&browser_activity);
}

void GUI_Deinit()
{
    GUI_DeinitImages();
    GUI_DeinitShaders();
    GUI_DeinitLib();
}