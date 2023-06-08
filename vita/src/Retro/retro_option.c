#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "List/option_list.h"
#include "Setting/setting.h"
#include "Emu/emu.h"
#include "retro.h"
#include "config.h"

int Retro_GetVariable(struct retro_variable *var)
{
    if (!var || core_option_list.length <= 0)
        return -1;

    OptionListEntry *entry = core_option_list.head;

    while (entry)
    {
        if (strcmp(var->key, entry->key) == 0)
        {
            var->value = entry->values[entry->sel_pos].value;
            break;
        }
        entry = entry->next;
    }

    return 0;
}

int Retro_SetCoreOptionDisplay(const struct retro_core_option_display *option_display)
{
    if (!option_display || core_option_list.length <= 0)
        return -1;

    OptionListEntry *entry = core_option_list.head;

    while (entry)
    {
        if (strcmp(option_display->key, entry->key) == 0)
        {
            *(entry->visible) = option_display->visible;
            break;
        }
        entry = entry->next;
    }

    if (entry != NULL)
        Setting_RequestRefreshOptionDisplay();

    return 0;
}

int Retro_GetOptionListFromVariables(struct retro_variable *varialbes)
{
    Setting_SetCoreMenu(NULL);
    OptionListEmpty(&core_option_list);

    if (!varialbes)
        return -1;

    int i, j, k;
    for (i = 0; varialbes[i].key; i++)
    {
        OptionListEntry *entry = (OptionListEntry *)calloc(1, sizeof(OptionListEntry));
        if (!entry)
            continue;
        OptionListAddEntry(&core_option_list, entry);

        // Get key
        entry->key = (char *)malloc(strlen(varialbes[i].key) + 1);
        if (entry->key)
            strcpy(entry->key, varialbes[i].key);
        // printf("Option key: %s\n", entry->key);

        if (!varialbes[i].value)
            continue;
        // printf("Option values: %s\n", varialbes[i].value);

        const char *values = varialbes[i].value;
        int values_len = strlen(values);
        int n_values = 1;
        const char *p, *sub;
        int len;

        p = values;
        while (*p == ' ')
            p++;

        // Get desc
        sub = strchr(p, ';');
        if (sub)
        {
            len = sub - p;
            entry->desc = (char *)malloc(len + 1);
            strncpy(entry->desc, p, len);
            entry->desc[len] = '\0';
            TrimString(entry->desc);
            p = sub + 1;
            while (*p == ' ')
                p++;
        }

        // Get values
        len = values + values_len - p;
        for (j = 0; j < len; j++)
        {
            if (p[j] == '|')
                n_values++;
        }

        entry->values = (OptionValue *)calloc(n_values, sizeof(OptionValue));
        if (!entry->values)
            continue;
        entry->n_values = n_values;

        for (j = 0; j < n_values; j++)
        {
            sub = strchr(p, '|');
            if (!sub)
                sub = values + values_len;
            len = sub - p;
            entry->values[j].value = (char *)malloc(len + 1);
            strncpy(entry->values[j].value, p, len);
            entry->values[j].value[len] = '\0';
            TrimString(entry->values[j].value);
            p = sub + 1;
            while (*p == ' ')
                p++;
        }

        // Get default value, in the index 0 of values
        entry->default_value = malloc(strlen(entry->values[0].value + 1));
        if (entry->default_value)
            strcpy(entry->default_value, entry->values[0].value);
    }

    Setting_SetCoreMenu(&core_option_list);
    Retro_UpdateCoreOptionsDisplay();

    if (Emu_IsGameLoading() || Emu_IsGameLoaded())
        LoadCoreConfig(TYPE_CONFIG_GAME);
    else
        LoadCoreConfig(TYPE_CONFIG_MAIN);

    return 0;
}

int Retro_GetOptionListFromOptions(void *options, int type)
{
    Setting_SetCoreMenu(NULL);
    OptionListEmpty(&core_option_list);

    if (!options)
        return -1;

    int option_version = 0;
    void *option_definition = NULL;
    int n_options = 0;

    if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS || type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL)
    {
        option_version = 1;
        struct retro_core_option_definition *option_v1_definition = NULL;
        if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL)
        {
            struct retro_core_options_intl *options_v1_intl = (struct retro_core_options_intl *)options;
            if (options_v1_intl->local)
                option_v1_definition = options_v1_intl->local;
            else if (options_v1_intl->us)
                option_v1_definition = options_v1_intl->us;
        }
        else
        {
            option_v1_definition = (struct retro_core_option_definition *)options;
        }

        if (!option_v1_definition)
            return -1;

        while (option_v1_definition[n_options].key)
        {
            n_options++;
        }

        option_definition = option_v1_definition;
    }
    else if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2 || type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL)
    {
        option_version = 2;
        struct retro_core_option_v2_definition *option_v2_definition = NULL;
        if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL)
        {
            struct retro_core_options_v2_intl *options_v2_intl = (struct retro_core_options_v2_intl *)options;
            if (options_v2_intl->local && options_v2_intl->local->definitions)
                option_v2_definition = options_v2_intl->local->definitions;
            else if (options_v2_intl->us && options_v2_intl->us->definitions)
                option_v2_definition = options_v2_intl->us->definitions;
        }
        else
        {
            struct retro_core_options_v2 *options_v2 = (struct retro_core_options_v2 *)options;
            option_v2_definition = options_v2->definitions;
        }

        if (!option_v2_definition)
            return -1;

        while (option_v2_definition[n_options].key)
        {
            n_options++;
        }

        option_definition = option_v2_definition;
    }
    else
    {
        return -1;
    }

    if (option_definition == NULL || n_options == 0 || option_version == 0)
        return -1;

    int i, j;
    for (i = 0; i < n_options; i++)
    {
        OptionListEntry *entry = (OptionListEntry *)calloc(1, sizeof(OptionListEntry));
        if (!entry)
            continue;
        OptionListAddEntry(&core_option_list, entry);

        const char *key;
        const char *desc;
        const char *default_value;
        struct retro_core_option_value *values;

        if (option_version == 1)
        {
            struct retro_core_option_definition *options = (struct retro_core_option_definition *)option_definition;
            key = options[i].key;
            desc = options[i].desc;
            default_value = options[i].default_value;
            values = options[i].values;
        }
        else
        {
            struct retro_core_option_v2_definition *options = (struct retro_core_option_v2_definition *)option_definition;
            key = options[i].key;
            desc = options[i].desc;
            default_value = options[i].default_value;
            values = options[i].values;
        }

        if (key)
        {
            // printf("Option key: %s\n", key);
            entry->key = (char *)malloc(strlen(key) + 1);
            if (entry->key)
                strcpy(entry->key, key);
        }

        if (desc)
        {
            // printf("Option desc: %s\n", desc);
            entry->desc = (char *)malloc(strlen(desc) + 1);
            if (entry->desc)
                strcpy(entry->desc, desc);
        }

        if (default_value)
        {
            // printf("Option default_value: %s\n", default_value);
            entry->default_value = (char *)malloc(strlen(default_value) + 1);
            if (entry->default_value)
                strcpy(entry->default_value, default_value);
        }

        if (!values)
            continue;

        int n_values = 0;
        while (values[n_values].value)
            n_values++;

        // printf("Option n_values: %d\n", n_values);

        if (n_values == 0)
            continue;

        entry->values = (OptionValue *)calloc(n_values, sizeof(OptionValue));
        if (!entry->values)
            continue;
        entry->n_values = n_values;

        int sel_pos_geted = 0;

        for (j = 0; j < n_values; j++)
        {
            if (values[j].value)
            {
                // printf("Core option value: value: %s\n", values[j].value);
                if (!sel_pos_geted && default_value && strcmp(values[j].value, default_value) == 0)
                {
                    sel_pos_geted = 1;
                    entry->sel_pos = j;
                }

                entry->values[j].value = (char *)malloc(strlen(values[j].value) + 1);
                if (entry->values[j].value)
                    strcpy(entry->values[j].value, values[j].value);
            }

            if (values[j].label)
            {
                entry->values[j].label = (char *)malloc(strlen(values[j].label) + 1);
                if (entry->values[j].label)
                    strcpy(entry->values[j].label, values[j].label);
            }
        }
    }

    Setting_SetCoreMenu(&core_option_list);
    Retro_UpdateCoreOptionsDisplay();

    if (Emu_IsGameLoading() || Emu_IsGameLoaded())
        LoadCoreConfig(TYPE_CONFIG_GAME);
    else
        LoadCoreConfig(TYPE_CONFIG_MAIN);

    return 0;
}
