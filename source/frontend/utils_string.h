#ifndef __M_UTILS_STRING_H__
#define __M_UTILS_STRING_H__

void TrimString(char *str);
char *TrimStringEx(char *str);

int StringToDecimal(const char *str);
int StringToHexdecimal(const char *str);
int StringToBoolean(const char *str);

int StringGetLine(const char *buf, int size, char **pline);
int StringReadConfigLine(const char *line, char **pkey, char **pvalue);

char *StringMakeShortByWidth(const char *string, int limit_w);
char *StringBreakLineByWidth(const char *string, int limit_w);

#endif
