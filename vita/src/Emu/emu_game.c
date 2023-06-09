#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/io/fcntl.h>

#include "Activity/browser.h"
#include "Setting/setting.h"
#include "Retro/retro.h"
#include "emu.h"
#include "file.h"
#include "config.h"
#include "boot.h"
#include "utils.h"
#include "log_scr.h"

enum RetroRequestEventTypes
{
    REQUEST_EVENT_TYPE_NONE,
    REQUEST_EVENT_TYPE_SAVE_STATE,
    REQUEST_EVENT_TYPE_LOAD_STATE,
    REQUEST_EVENT_TYPE_RESET,
    REQUEST_EVENT_TYPE_EXIT,
};

extern GUI_Dialog setting_dialog;

static int game_loading = 0, game_reloading = 0, game_loaded = 0, game_run = 0;
static int request_event_type = REQUEST_EVENT_TYPE_NONE;
static float game_run_speed = 1.0f;
static double game_cur_fps = 0;
static void *game_rom_data = NULL;

int Emu_IsGameRun()
{
    return game_run;
}

int Emu_IsGameLoading()
{
    return game_loading;
}

int Emu_IsGameLoaded()
{
    return game_loaded;
}

double Emu_GetCurrentFps()
{
    return game_cur_fps;
}

float Emu_GetCurrentRunSpeed()
{
    return game_run_speed;
}

void Emu_SetRunSpeed(float speed)
{
    game_run_speed = speed;
    game_cur_fps = core_system_av_info.timing.fps * (double)speed;
    Emu_SetMicrosPerFrame(1000000.0f / game_cur_fps);
}

static int Emu_LoadSrm()
{
    size_t dst_size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    if (dst_size == 0)
        return -1;

    void *dst_data = retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    if (dst_data == NULL)
        return -1;

    char path[MAX_PATH_LENGTH];
    MakeSrmPath(path);
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
        return fd;

    int64_t src_size = sceIoLseek(fd, 0, SCE_SEEK_END);
    if (src_size <= 0 || src_size > dst_size)
    {
        AppLog("Emu_LoadSrm failed: src_size %d, dst_size %d\n", src_size, dst_size);
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

static int Emu_SaveSrm()
{
    size_t size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    if (size == 0)
        return -1;

    void *data = retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    if (data == NULL)
        return -1;

    char path[MAX_PATH_LENGTH];
    MakeSrmPath(path);
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

static int loadGameFromFile(const char *path)
{
    struct retro_game_info game_info;
    game_info.path = path;
    game_info.data = NULL;
    game_info.size = 0;
    game_info.meta = NULL;
    int ret = retro_load_game(&game_info);
    if (!ret)
        return -1;

    return 0;
}

static int loadGameFromMemory(const char *path)
{
    SceUID fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0)
        return -1;

    int64_t size = sceIoLseek(fd, 0, SCE_SEEK_END);
    if (size <= 0)
    {
        sceIoClose(fd);
        return -1;
    }

    if (game_rom_data)
        free(game_rom_data);
    game_rom_data = (void *)malloc(size);
    if (!game_rom_data)
    {
        sceIoClose(fd);
        return -1;
    }

    sceIoLseek(fd, 0, SCE_SEEK_SET);
    char *buf = (char *)game_rom_data;
    int64_t remain = size;
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
            free(game_rom_data);
            game_rom_data = NULL;
            sceIoClose(fd);
            return -1;
        }
        if (read == 0)
            break;

        buf += read;
        remain -= read;
    }
    sceIoClose(fd);

    struct retro_game_info game_info;
    game_info.path = path;
    game_info.data = game_rom_data;
    game_info.size = size;
    game_info.meta = NULL;

    int ret = retro_load_game(&game_info);
    if (!ret)
    {
        free(game_rom_data);
        game_rom_data = NULL;
        return -1;
    }

    return 0;
}

int Emu_LoadGame(const char *path)
{
    int ret;
    int show_log;

    AppLog("[GAME] Load game...\n");
    AppLog("[GAME] Rom path: %s\n", path);

    game_loading = 1;
    GUI_DisplaySplash();

    LoadGraphicsConfig(TYPE_CONFIG_GAME);
    LoadControlConfig(TYPE_CONFIG_GAME);
    LoadMiscConfig(TYPE_CONFIG_GAME);
    LoadCoreConfig(TYPE_CONFIG_GAME);

    show_log = app_config.print_log;
    if (show_log)
    {
        LogScrSetAutoScroll(1);
        LogScrInit();
    }

    retro_init();

    AppLog("[GAME] Core need full path: %d\n", core_system_info.need_fullpath);
    if (core_system_info.need_fullpath)
        ret = loadGameFromFile(path);
    else
        ret = loadGameFromMemory(path);

    game_loading = 0;

    if (ret < 0)
    {
        AppLog("[GAME] Load game failed!\n");
        Emu_ExitGame();
        if (show_log)
        {
            LogScrSetAutoScroll(0);
            LogScrWaitExit();
        }
        return ret;
    }

    WriteFile((LASTFILE_PATH), path, strlen(path) + 1);
    if (show_log)
        LogScrDeinit();

    request_event_type = REQUEST_EVENT_TYPE_NONE;
    retro_get_system_av_info(&core_system_av_info);
    Emu_SetRunSpeed(1.0f);

    Retro_SetControllerPortDevices();
    Emu_LoadSrm();
    retro_run();

    Emu_InitAudio();
    Emu_InitVideo();
    Emu_InitInput();

    game_loaded = 1;
    GUI_CleanPad();
    Emu_RequestRefreshVideo(1);
    Emu_ResumeGame();
    Retro_UpdateCoreOptionsDisplay();
    Setting_RequestRefreshMenu();

    AppLog("[GAME] Load game done\n");

    return ret;
}

void Emu_ExitGame()
{
    if (game_loaded)
    {
        Emu_PauseGame();
        Emu_SaveSrm();
        if (!game_reloading && misc_config.auto_save_load)
        {
            Emu_SaveState(-1);
            Browser_RequestRefreshPreview(1);
        }
        retro_unload_game();
        retro_deinit();
        game_loaded = 0;
        Emu_DeinitAudio();
        Emu_DeinitVideo();
        Emu_DeinitInput();
    }

    if (game_rom_data)
    {
        free(game_rom_data);
        game_rom_data = NULL;
    }

    if (!game_reloading)
    {
        LoadGraphicsConfig(TYPE_CONFIG_MAIN);
        LoadControlConfig(TYPE_CONFIG_MAIN);
        LoadMiscConfig(TYPE_CONFIG_MAIN);
        LoadCoreConfig(TYPE_CONFIG_MAIN);
        Retro_UpdateCoreOptionsDisplay();
        Setting_RequestRefreshMenu();
    }
}

void Emu_PauseGame()
{
    if (game_loaded)
    {
        game_run = 0;
        Emu_PauseAudio();
        Emu_PauseVideo();
        if (request_event_type == REQUEST_EVENT_TYPE_NONE)
            UnlockQuickMenu();
    }
}

void Emu_ResumeGame()
{
    if (game_loaded)
    {
        game_run = 1;
        Emu_ResumeAudio();
        Emu_ResumeVideo();
        LockQuickMenu();
    }
}

void Emu_ResetGame()
{
    if (game_loaded)
    {
        retro_reset();
        Emu_CleanAudioSound();
    }
}

int Emu_ReloadGame()
{
    game_reloading = 1;
    if (game_loaded)
        Emu_ExitGame();
    char path[MAX_PATH_LENGTH];
    MakeCurrentFilePath(path);
    int ret = Emu_LoadGame(path);
    game_reloading = 0;

    return ret;
}

static void checkRequestsEvents()
{
    if (request_event_type == REQUEST_EVENT_TYPE_NONE)
        return;

    if (request_event_type == REQUEST_EVENT_TYPE_SAVE_STATE)
    {
        Emu_PauseGame();
        Emu_SaveState(Setting_GetStateSelectId());
        Emu_ResumeGame();
    }
    else if (request_event_type == REQUEST_EVENT_TYPE_LOAD_STATE)
    {
        Emu_PauseGame();
        Emu_LoadState(Setting_GetStateSelectId());
        Emu_ResumeGame();
    }
    else if (request_event_type == REQUEST_EVENT_TYPE_RESET)
    {
        Emu_PauseGame();
        Emu_ResetGame();
        Emu_ResumeGame();
    }
    else if (request_event_type == REQUEST_EVENT_TYPE_EXIT)
    {
        Emu_ExitGame();
        if (exec_boot_mode == BOOT_MODE_GAME)
            BootReturnToParent();
    }

    request_event_type = REQUEST_EVENT_TYPE_NONE;
}

void Emu_RunGame()
{
    Emu_PollInput();
    retro_run();
    checkRequestsEvents();
}

void Emu_SpeedUpGame()
{
    float speed = Emu_GetCurrentRunSpeed();
    if (speed < 2.0f)
        speed += 0.5f;
    else
        speed = 1.0f;
    Emu_SetRunSpeed(speed);
}

void Emu_SpeedDownGame()
{
    float speed = Emu_GetCurrentRunSpeed();
    if (speed > 1.0f)
        speed -= 0.5f;
    else
        speed = 2.0f;
    Emu_SetRunSpeed(speed);
}

void Emu_RequestResetGame()
{
    request_event_type = REQUEST_EVENT_TYPE_RESET;
}

void Emu_RequestExitGame()
{
    request_event_type = REQUEST_EVENT_TYPE_EXIT;
}

void Emu_RequestSaveState()
{
    request_event_type = REQUEST_EVENT_TYPE_SAVE_STATE;
}

void Emu_RequestLoadState()
{
    request_event_type = REQUEST_EVENT_TYPE_LOAD_STATE;
}
