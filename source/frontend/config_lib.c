#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void TrimString(char *str)
{
    int len = strlen(str);
    int i;

    for (i = len - 1; i >= 0; i--)
    {
        if (str[i] == ' ' || str[i] == '\t')
            str[i] = 0;
        else
            break;
    }
}

char *TrimStringEx(char *str)
{
    char *res = str;
    while (*res == ' ' || *res == '\t')
        res++;
    TrimString(res);

    return res;
}

int StringToDecimal(const char *str)
{
    return strtol(str, NULL, 0);
}

int StringToHexdecimal(const char *str)
{
    return strtoul(str, NULL, 16);
}

int StringToBoolean(const char *str)
{
    if (strcasecmp(str, "false") == 0 ||
        strcasecmp(str, "off") == 0 ||
        strcasecmp(str, "no") == 0)
        return 0;

    if (strcasecmp(str, "true") == 0 ||
        strcasecmp(str, "on") == 0 ||
        strcasecmp(str, "yes") == 0)
        return 1;

    return -1;
}

int ConfigGetLine(const char *buf, int size, char **pline)
{
    int n = 0;
    int i = 0;
    uint8_t *p = (uint8_t *)buf;

    for (i = 0; i < size; i++)
    {
        if (p[i] < 0x20 && p[i] != '\t')
        {
            i++;
            break;
        }
        n++;
    }

    if (n > 0)
    {
        *pline = malloc(n + 1);
        strncpy(*pline, buf, n);
        (*pline)[n] = '\0';
    }

    return i;
}

int ConfigReadLine(const char *line, char **pkey, char **pvalue)
{
    if (!line)
        return -1;

    char *key = NULL;
    char *value = NULL;
    const char *head = line;
    const char *tail = line + strlen(line);
    int len;

    // Trim at beginning
    while (*head == ' ' || *head == '\t')
        head++;

    // Ignore comments #1
    if (head[0] == '#')
    {
        // printf("IGNORE %s\n", line);
        goto FAILED;
    }

    // Ignore comments #2
    const char *p = strchr(head, '#');
    if (p)
    {
        // printf("IGNORE %s\n", p);
        tail = p;
    }

    // Get token
    p = strchr(head, '=');
    if (!p || p >= tail)
        goto FAILED;

    // Key
    len = p - head;
    key = malloc(len + 1);
    if (!key)
        goto FAILED;
    strncpy(key, head, len);
    key[len] = '\0';
    TrimString(key);
    // printf("KEY: %s\n", key);

    head = p + 1;
    while (*head == ' ' || *head == '\t')
        head++;

    if (head[0] == '"') // String
    {
        head++;
        p = strchr(head, '"');
        if (!p || p >= tail)
            goto FAILED;
    }
    else // Decimal
    {
        p = tail;
        while (p > head && (*(p - 1) == ' ' || *(p - 1) == '\t'))
            p--;
    }

    // Value
    len = p - head;
    value = malloc(len + 1);
    if (!value)
        goto FAILED;
    strncpy(value, head, len);
    value[len] = '\0';
    // printf("VALUE: %s\n", value);

    *pkey = key;
    *pvalue = value;
    return 0;

FAILED:
    if (key)
        free(key);
    if (value)
        free(value);
    return -1;
}
