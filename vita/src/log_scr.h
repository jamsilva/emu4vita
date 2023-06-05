#ifndef __M_LOG_SCR_H__
#define __M_LOG_SCR_H__

int LogScrInit();
int LogScrWaitExit();
int LogScrDeinit();

void LogScrPrintText(const char *text);
void LogScrPrintfText(const char *text, ...);

void LogScrSetAutoScroll(int enable);

#endif