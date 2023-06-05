#ifndef __M_BOOT_H__
#define __M_BOOT_H__

enum BootModes
{
    BOOT_MODE_NONE,
	BOOT_MODE_GAME,
    BOOT_MODE_ARCH,
};

int sendBootParams(char *app_path, char *argv[]);
int loadGameWithBootParams(char *app_path, char *game_path, char *software);
int loadCoreWithBootParams(char *app_path, char *software);

#endif