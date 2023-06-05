#ifndef __M_BROWSER_H__
#define __M_BROWSER_H__

#include "Gui/gui.h"

int CurrentPathIsFile();
int MakeCurrentFileName(char *name);
int MakeCurrentFilePath(char *path);
int MakePreviewPath(char *path);
int MakeScreenshotPath(char *path);

GUI_Texture *GetDefaultPreviewTexture();

void Browser_RequestRefreshPreview(int urgent);

int Browser_ChangeDirByPath(const char *path);
int Browser_ChangeDirByFile(const char *path);

#endif
