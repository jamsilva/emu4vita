#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/appmgr.h>

#include "activity/browser.h"
#include "emu/emu.h"
#include "boot.h"
#include "utils.h"
#include "file.h"
#include "config.h"

#define EXEC_BOOT_MODE_CASE_STR "boot_mode"
#define BOOT_GAME_PATH_CASE_STR "game_path"
#define PRIVATE_ASSETS_CASE_STR "core_assets_dir"
#define PUBLIC_ASSETS_CASE_STR "common_assets_dir"
#define RESTORE_APP_PATH_CASE_STR "restore_app_path"

int exec_boot_mode = 0;
static char *boot_game_path = NULL;
static char *restore_app_path = NULL;

static int bootparam_argc = 0;
static char **bootparam_argv = NULL;

int BootLoadGame()
{
    if (!boot_game_path)
        return -1;

    Browser_ChangeDirByFilePath(boot_game_path);

    EmuGameInfo info;
    strcpy(info.path, boot_game_path);
    info.state_num = -2;
    Emu_StartGame(&info);

    return 0;
}

static int readBootParamString(const char *str)
{
    char *name = NULL;
    char *value = NULL;
    int ret = StringReadConfigLine(str, &name, &value);
    if (ret < 0)
        return ret;

    if (strcasecmp(name, EXEC_BOOT_MODE_CASE_STR) == 0)
    {
        exec_boot_mode = StringToDecimal(value);
    }
    else if (strcasecmp(name, BOOT_GAME_PATH_CASE_STR) == 0)
    {
        boot_game_path = value;
        value = NULL;
    }
    else if (strcasecmp(name, PRIVATE_ASSETS_CASE_STR) == 0)
    {
        private_assets_dir = value;
        value = NULL;
    }
    else if (strcasecmp(name, PUBLIC_ASSETS_CASE_STR) == 0)
    {
        public_assets_dir = value;
        value = NULL;
    }
    else if (strcasecmp(name, RESTORE_APP_PATH_CASE_STR) == 0)
    {
        restore_app_path = value;
        value = NULL;
    }

    if (name)
        free(name);
    if (value)
        free(value);

    return 0;
}

int BootCheckParams(int argc, char *const argv[])
{
    if (argc < 2)
        goto END;

    AppLog("[BOOT] argc: %d\n", argc);
    bootparam_argc = argc;
    bootparam_argv = (char **)calloc(argc, sizeof(char *));

    int i, len;
    for (i = 1; i < argc; i++)
    {
        len = strlen(argv[i]);
        bootparam_argv[i - 1] = (char *)malloc(len + 1);
        strcpy(bootparam_argv[i - 1], argv[i]);
        readBootParamString(argv[i]);
        AppLog("[BOOT] argv[%d]: %s\n", i, argv[i]);
    }
    bootparam_argv[bootparam_argc - 1] = NULL;

END:
    if (!public_assets_dir)
    {
        public_assets_dir = (char *)malloc(MAX_PATH_LENGTH);
        strcpy(public_assets_dir, APP_ASSETS_DIR);
    }
    AppLog("[BOOT] Set public assets dir: %s\n", public_assets_dir);

    return 0;
}

int BootLoadExec(char *app_path, char *argv[])
{
    int ret = sceAppMgrLoadExec(app_path, (char *const *)argv, NULL);

    return ret;
}

int BootReturnToParent()
{
    if (exec_boot_mode <= 0)
        return -1;

    char *app_path;
    if (restore_app_path)
        app_path = restore_app_path;
    else
        app_path = "app0:eboot.bin";
    AppLog("[BOOT] Return to %s\n", app_path);

    int ret = BootLoadExec(app_path, bootparam_argv);

    return ret;
}

int LoadGameWithBootParams(char *app_path, char *game_path, char *software)
{
    if (!CheckFileExist(app_path))
        return -1;

    char boot_mode_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(boot_mode_param, MAX_CONFIG_LINE_LENGTH, "%s=%d", EXEC_BOOT_MODE_CASE_STR, BOOT_MODE_GAME);

    char game_path_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(game_path_param, MAX_CONFIG_LINE_LENGTH, "%s=\"%s\"", BOOT_GAME_PATH_CASE_STR, game_path);

    char private_assets_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(private_assets_param, MAX_CONFIG_LINE_LENGTH, "%s=\"%s/%s\"", PRIVATE_ASSETS_CASE_STR, CORE_DATA_DIR, software);

    char public_assets_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(public_assets_param, MAX_CONFIG_LINE_LENGTH, "%s=\"%s\"", PUBLIC_ASSETS_CASE_STR, APP_ASSETS_DIR);

    char *argv[] = {
        boot_mode_param,
        game_path_param,
        private_assets_param,
        public_assets_param,
        NULL,
    };
    int ret = BootLoadExec(app_path, argv);

    return ret;
}

int LoadCoreWithBootParams(char *app_path, char *software)
{
    if (!CheckFileExist(app_path))
        return -1;

    char boot_mode_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(boot_mode_param, MAX_CONFIG_LINE_LENGTH, "%s=%d", EXEC_BOOT_MODE_CASE_STR, BOOT_MODE_ARCH);

    char private_assets_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(private_assets_param, MAX_CONFIG_LINE_LENGTH, "%s=\"%s/%s\"", PRIVATE_ASSETS_CASE_STR, CORE_DATA_DIR, software);

    char public_assets_param[MAX_CONFIG_LINE_LENGTH];
    snprintf(public_assets_param, MAX_CONFIG_LINE_LENGTH, "%s=\"%s\"", PUBLIC_ASSETS_CASE_STR, APP_ASSETS_DIR);

    char *argv[] = {
        boot_mode_param,
        private_assets_param,
        public_assets_param,
        NULL,
    };
    int ret = BootLoadExec(app_path, argv);

    return ret;
}