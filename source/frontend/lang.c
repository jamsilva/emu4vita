#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "lang.h"
#include "lang_intl.h"
#include "init.h"

LangEntry lang_entries[] = {
    {"English", lang_us},
    {"Japanese", NULL},
    {"French", NULL},
    {"Spanish", NULL},
    {"German", NULL},
    {"Italian", NULL},
    {"Dutch", NULL},
    {"Portuguese(brazil)", NULL},
    {"Portuguese(portugal)", NULL},
    {"Russian", NULL},
    {"Korean", NULL},
    {"繁體中文", NULL},
    {"简体中文", lang_chs},
    {"Esperanto", NULL},
    {"Polish", NULL},
    {"Vietnamese", NULL},
    {"Arabic", NULL},
    {"Greek", NULL},
    {"Turkish", NULL},
    {"Slovak", NULL},
    {"Persian", NULL},
    {"Hebrew", NULL},
    {"Asturian", NULL},
    {"Finnish", NULL},
};

#define N_LANGS (sizeof(lang_entries) / sizeof(LangEntry))

char **cur_lang = lang_us;

int GetLangsLength()
{
    return N_LANGS;
}

int GetLangValue(int fake_id)
{
    int index = 0;
    int length = N_LANGS;

    int i;
    for (i = 0; i < length; i++)
    {
        if (lang_entries[i].container)
        {
            if (index == fake_id)
                break;
            else
                index++;
        }
    }
    if (i >= length) // No finded
        return 0;    // Use default us

    return i;
}

int SetCurrentLang(int fake_id)
{
    int index = 0;
    int length = N_LANGS;

    int i;
    for (i = 0; i < length; i++)
    {
        if (lang_entries[i].container)
        {
            if (index == fake_id)
                break;
            else
                index++;
        }
    }
    if (i >= length) // No finded
        return -1;

    cur_lang = lang_entries[i].container;
    return 0;
}

char *GetLangString(LangString *lang_s)
{
    if (!lang_s)
        return NULL;

    if (cur_lang && lang_s->lang != LANG_NULL && lang_s->lang < LANGUAGE_CONTAINER_SIZE)
        return cur_lang[lang_s->lang];
    else
        return lang_s->string;
}
