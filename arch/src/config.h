#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__

#define APP_DATA_DIR        "ux0:data/EMU4VITA/Arch"
#define APP_ASSETS_DIR      "app0:assets"
#define CORE_DATA_DIR       "app0:data"

#define FONT_PGF_PATH        APP_ASSETS_DIR "/font.pgf"
#define WALLPAPER_PNG_PATH   APP_ASSETS_DIR "/bg.png"
#define SPLASH_PNG_PATH      APP_ASSETS_DIR "/splash.png"

#define APP_LOG_PATH         APP_DATA_DIR "/log.txt"
#define LASTFILE_PATH        APP_DATA_DIR "/lastfile.txt"

#define MAX_CONFIG_NAME_LENGTH   128
#define MAX_CONFIG_STRING_LENGTH 1024
#define MAX_CONFIG_LINE_LENGTH   2048

void trimString(char *str);
char *trimStringEx(char *str);

int configGetDecimal(const char *str);
int configGetHexdecimal(const char *str);
int configGetBoolean(const char *str);
char *configGetString(const char *str);

int configGetLine(const char *buf, int size, char **line);
int configReadLine(const char *line, char **name, char **string);
/*
extern char *private_assets_dir;
extern char *public_assetss_dir;
*/
#endif
