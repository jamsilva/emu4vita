#ifndef __M_CONFIG_TEXT_H__
#define __M_CONFIG_TEXT_H__

void TrimString(char *str);
char *TrimStringEx(char *str);

int StringToDecimal(const char *str);
int StringToHexdecimal(const char *str);
int StringToBoolean(const char *str);

int ConfigGetLine(const char *buf, int size, char **line);
int ConfigReadLine(const char *line, char **pkey, char **pvalue);

#endif