#ifndef __M_BROWSER_H__
#define __M_BROWSER_H__

#include <vita2d.h>

typedef struct
{
	char *desc;
    char *core_name;
} CoreEntry;

typedef struct
{
	char *software;
	vita2d_texture *icon;
	CoreEntry *entry;
    int n_entries;
} BrowserEntry;

void initIconsThread();

void initBrowserDrawInfo();
int initBrowser();
int drawBrowser();
int ctrlBrowser();

#endif
