#ifndef __M_FILE_H__
#define __M_FILE_H__

#define MAX_PATH_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define MAX_SHORT_NAME_LENGTH 64

int ReadFile(const char *file, void *buf, int size);
int WriteFile(const char *file, const void *buf, int size);
int allocateReadFile(const char *file, void **buffer);

int checkFileExist(const char *file);
int checkFolderExist(const char *folder);

int createFolder(const char *path);

char *getBaseDirectory(const char *path);
char *getFilename(const char *path);
char *getBaseFilename(const char *path);

int getFileSize(const char *file);

#endif
