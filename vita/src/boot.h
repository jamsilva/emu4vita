#ifndef __M_BOOT_H__
#define __M_BOOT_H__

enum BootModes
{
    BOOT_MODE_NONE,
	BOOT_MODE_GAME,
    BOOT_MODE_ARCH,
};

int BootCheckParams(int argc, char *const argv[]);
int BootLoadExec(char *app_path, char *argv[]);
int BootReturnToParent();

int BootLoadGame();
int LoadGameWithBootParams(char *app_path, char *game_path, char *software);
int LoadCoreWithBootParams(char *app_path, char *software);

extern int exec_boot_mode;

#endif