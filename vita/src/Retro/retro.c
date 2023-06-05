#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <pthread.h>
#include <time/rtime.h>

#include "Activity/browser.h"
#include "Emu/emu.h"
#include "retro.h"
#include "utils.h"
#include "file.h"
#include "config.h"
#include "lang.h"

extern bool Retro_EnvironmentCallback(unsigned int cmd, void *data);
extern size_t Retro_AudioSampleBatchCallback(const int16_t *data, size_t frames);
extern void Retro_VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
extern void Retro_InputPollCallback();
extern int16_t Retro_InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);

char core_assets_dir[MAX_PATH_LENGTH];
char core_system_dir[MAX_PATH_LENGTH];

struct retro_system_info core_system_info;
struct retro_system_av_info core_system_av_info;

unsigned core_option_language = RETRO_LANGUAGE_CHINESE_SIMPLIFIED;
static unsigned int core_device = RETRO_DEVICE_JOYPAD;

int MakeCoreAssetsDirPath(char *path)
{
    snprintf(path, MAX_PATH_LENGTH, "%s/%s", APP_DATA_DIR, CORE_ASSETS_DIR_NAME);
    return 0;
}

int MakeCoreSystemDirPath(char *path)
{
#if defined(USE_INTE_SYSTEM_DIR)
    if (private_assets_dir)
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", private_assets_dir, CORE_SYSTEM_DIR_NAME);
    else
        snprintf(path, MAX_PATH_LENGTH, "%s/%s", public_assets_dir, CORE_SYSTEM_DIR_NAME);
#else
    snprintf(path, MAX_PATH_LENGTH, "%s/%s", APP_DATA_DIR, CORE_SYSTEM_DIR_NAME);
#endif
    return 0;
}

int MakeSrmPath(char *path)
{
    char name[MAX_NAME_LENGTH];
    MakeCurrentFileName(name);
    char base_name[MAX_NAME_LENGTH];
    MakeBaseName(base_name, name, MAX_NAME_LENGTH);
    snprintf(path, MAX_PATH_LENGTH, "%s/%s.srm", (CORE_SAVEFILES_DIR), base_name);
    return 0;
}

#if defined(COPY_INTE_SYSTEM_DIR)
static int CopyInternalSystemDir()
{
    char src_path[MAX_PATH_LENGTH];
    if (private_assets_dir)
        snprintf(src_path, MAX_PATH_LENGTH, "%s/%s", private_assets_dir, CORE_SYSTEM_DIR_NAME);
    else
        snprintf(src_path, MAX_PATH_LENGTH, "%s/%s", public_assets_dir, CORE_SYSTEM_DIR_NAME);

    if (!CheckFileExist(src_path))
        return -1;

    char dst_path[MAX_PATH_LENGTH];
    snprintf(dst_path, MAX_PATH_LENGTH, "%s/%s", APP_DATA_DIR, CORE_SYSTEM_DIR_NAME);

    CreateFolder(dst_path);
    CopyPath(src_path, dst_path);

    return 0;
}
#endif

static void freeValidFileExts()
{
    if (file_valid_exts)
    {
        int i = 0;
        while (file_valid_exts[i])
        {
            free(file_valid_exts[i]);
            i++;
        }
        free(file_valid_exts);
        file_valid_exts = NULL;
    }
}

static int creatValidFileExts()
{
    int ret = 0;
    int i, j;
    const char *exts = core_system_info.valid_extensions;
    int exts_len;
    int n_exts;

    if (!exts)
        return -1;
    APP_LOG("[RETRO] Valid extensions: %s\n", exts);

    exts_len = strlen(exts);
    if (exts_len == 0)
        return -1;

    n_exts = 1;
    for (i = 0; i < exts_len; i++)
    {
        if (exts[i] == '|')
            n_exts++;
    }
    // APP_LOG("n_exts: %d\n", n_exts);

    if (file_valid_exts)
        freeValidFileExts();
    file_valid_exts = (char **)calloc((n_exts + 1), sizeof(char *));
    if (!file_valid_exts)
        return -1;

    const char *p = exts;
    const char *sep;
    int len;
    for (i = 0; i < n_exts; i++)
    {
        sep = strchr(p, '|');
        if (!sep)
            sep = exts + exts_len;
        len = sep - p;
        file_valid_exts[i] = (char *)malloc(len + 1);
        if (file_valid_exts[i])
        {
            strncpy(file_valid_exts[i], p, len);
            file_valid_exts[i][len] = '\0';
        }
        p = sep + 1;
    }
    file_valid_exts[n_exts] = NULL;

    // for (i = 0; i < n_exts; i++)
    //     APP_LOG("exts[%d]: %s\n", i, file_valid_exts[i]);

    return ret;
}

static void setRetroCallbacks()
{
    retro_set_environment(Retro_EnvironmentCallback);
    retro_set_video_refresh(Retro_VideoRefreshCallback);
    retro_set_audio_sample_batch(Retro_AudioSampleBatchCallback);
    retro_set_input_poll(Retro_InputPollCallback);
    retro_set_input_state(Retro_InputStateCallback);
}

void Retro_SetControllerPortDevices()
{
    int i;
    for (i = 0; i < 4; i++)
        retro_set_controller_port_device(i, core_device);
}

int Retro_InitLib()
{
    APP_LOG("[RETRO] Retro init lib...\n");

    pthread_init();
    rtime_init();

    MakeCoreAssetsDirPath(core_assets_dir);
    MakeCoreSystemDirPath(core_system_dir);

    core_options_update_display_callback = NULL;
    core_option_language = RETRO_LANGUAGE_CHINESE_SIMPLIFIED;
    core_device = RETRO_DEVICE_JOYPAD;

    setRetroCallbacks();
    retro_get_system_info(&core_system_info);
    creatValidFileExts();
    
#if defined(COPY_INTE_SYSTEM_DIR)
    CopyInternalSystemDir();
#endif

    APP_LOG("[RETRO] Retro init lib done\n");

    return 0;
}

int Retro_DeinitLib()
{
    APP_LOG("[RETRO] Retro deinit lib...\n");

    rtime_deinit();
    pthread_terminate();

    APP_LOG("[RETRO] Retro deinit lib done\n");

    return 0;
}
