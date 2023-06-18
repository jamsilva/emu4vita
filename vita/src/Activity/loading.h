#ifndef __M_LOADING_H__
#define __M_LOADING_H__

void Loading_AddLog(const char *text);
void Loading_AddLogf(const char *text, ...);
void Loading_SetAutoScrollListview(int enable);

void Loading_WaitActivityThreadEnd();
int Loading_StartActivityThread();
int Loading_ExitActivityThread();

#endif