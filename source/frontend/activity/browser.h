#ifndef __M_BROWSER_H__
#define __M_BROWSER_H__

#include "gui/gui.h"

int CurrentPathIsFile();
int MakeCurrentFileName(char *name);
int MakeCurrentFilePath(char *path);
int MakePreviewPath(char *path);
int MakeScreenshotPath(char *path);

GUI_Texture *GetDefaultPreviewTexture();

void Browser_RequestRefreshPreview(int urgent);

int Browser_ChangeDirByFilePath(const char *path);
int Browser_ChangeDirBySaveFile(const char *path);

#endif
