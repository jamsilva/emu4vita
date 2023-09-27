#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "list/option_list.h"
#include "setting/setting.h"
#include "emu/emu.h"
#include "utils_string.h"
#include "config.h"

int Retro_GetVariable(struct retro_variable *var)
{
    if (!var || !core_option_list)
        return -1;

    LinkedListEntry *entry = OptionListFindEntryByKey(core_option_list, var->key);
    if (!entry)
        return -1;

    OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);
    var->value = data->values[data->select].value;
    Setting_RequestUpdateMenuItems();

    return 0;
}

int Retro_SetCoreOptionsDisplay(const struct retro_core_option_display *option_display)
{
    if (!option_display || !core_option_list)
        return -1;

    LinkedListEntry *entry = OptionListFindEntryByKey(core_option_list, option_display->key);
    if (!entry)
        return -1;

    OptionListEntryData *data = (OptionListEntryData *)LinkedListGetEntryData(entry);
    data->visibility = option_display->visible;
    Setting_RequestUpdateMenuItems();

    return 0;
}

void Retro_UpdateCoreOptionsDisplay()
{
    if (core_options_update_display_callback && core_options_update_display_callback->callback)
    {
        core_options_update_display_callback->callback();
        Setting_RequestUpdateMenuItems();
    }
    else
    {
        Retro_RequestUpdateOptionsDisplay();
    }
}

int Retro_SetOptionListFromVariables(struct retro_variable *varialbes)
{
    Setting_SetCoreMenu(NULL);
    if (core_option_list)
        LinkedListDestroy(core_option_list);
    core_option_list = NULL;

    if (!varialbes)
        return -1;

    core_option_list = NewOptionList();
    if (!core_option_list)
        return -1;

    int i, j;
    for (i = 0; varialbes[i].key; i++)
    {
        OptionListEntryData *data = (OptionListEntryData *)calloc(1, sizeof(OptionListEntryData));
        if (!data)
            continue;
        LinkedListAdd(core_option_list, data);

        data->visibility = 1;

        // Get key
        data->key = (char *)malloc(strlen(varialbes[i].key) + 1);
        if (data->key)
            strcpy(data->key, varialbes[i].key);
        // printf("Option key: %s\n", data->key);

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
            data->desc = (char *)malloc(len + 1);
            strncpy(data->desc, p, len);
            data->desc[len] = '\0';
            TrimString(data->desc);
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

        if (n_values == 0)
            continue;

        data->values = (OptionValue *)calloc(n_values, sizeof(OptionValue));
        if (!data->values)
            continue;
        data->n_values = n_values;

        for (j = 0; j < n_values; j++)
        {
            sub = strchr(p, '|');
            if (!sub)
                sub = values + values_len;
            len = sub - p;
            data->values[j].value = (char *)malloc(len + 1);
            strncpy(data->values[j].value, p, len);
            data->values[j].value[len] = '\0';
            TrimString(data->values[j].value);
            p = sub + 1;
            while (*p == ' ')
                p++;
        }

        // Get default value, in the index 0 of values
        data->default_select = 0;
        data->select = data->default_select;
    }

    Setting_SetCoreMenu(core_option_list);
    if (!Emu_IsGameLoading())
        Retro_UpdateCoreOptionsDisplay();

    if (Emu_IsGameLoading() || Emu_IsGameLoaded())
        LoadCoreConfig(TYPE_CONFIG_GAME);
    else
        LoadCoreConfig(TYPE_CONFIG_MAIN);

    return 0;
}

int Retro_SetOptionListFromOptions(void *options, int type)
{
    Setting_SetCoreMenu(NULL);
    if (core_option_list)
        LinkedListDestroy(core_option_list);
    core_option_list = NULL;

    if (!options)
        return -1;

    core_option_list = NewOptionList();
    if (!core_option_list)
        return -1;

    int i, j, k;
    for (i = 0;; i++)
    {
        const char *key = NULL;
        const char *desc = NULL;
        const char *default_value = NULL;
        const struct retro_core_option_value *values = NULL;
        const struct retro_core_option_value *local_values = NULL;

        if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS)
        {
            struct retro_core_option_definition *definitions = (struct retro_core_option_definition *)options;
            if (!definitions[i].key)
                break;

            key = definitions[i].key;
            desc = definitions[i].desc;
            default_value = definitions[i].default_value;
            values = definitions[i].values;
        }
        else if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL)
        {
            struct retro_core_options_intl *options_intl = (struct retro_core_options_intl *)options;
            struct retro_core_option_definition *us_definitions = options_intl->us;
            struct retro_core_option_definition *local_definitions = options_intl->local;
            if (!us_definitions || !us_definitions[i].key)
                break;

            key = us_definitions[i].key;
            desc = us_definitions[i].desc;
            default_value = us_definitions[i].default_value;
            values = us_definitions[i].values;
            if (local_definitions)
            {
                for (j = 0; local_definitions[j].key; j++)
                {
                    if (strcmp(local_definitions[j].key, us_definitions[i].key) == 0)
                    {
                        if (local_definitions[j].desc)
                            desc = local_definitions[j].desc;
                        local_values = local_definitions[j].values;
                        break;
                    }
                }
            }
        }
        else if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2)
        {
            struct retro_core_options_v2 *options_v2 = (struct retro_core_options_v2 *)options;
            struct retro_core_option_v2_definition *definitions = options_v2->definitions;
            if (!definitions[i].key)
                break;

            key = definitions[i].key;
            desc = definitions[i].desc;
            default_value = definitions[i].default_value;
            values = definitions[i].values;
        }
        else if (type == RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL)
        {
            struct retro_core_options_v2_intl *options_v2_intl = (struct retro_core_options_v2_intl *)options;
            struct retro_core_option_v2_definition *us_definitions = options_v2_intl->us ? options_v2_intl->us->definitions : NULL;
            struct retro_core_option_v2_definition *local_definitions = options_v2_intl->local ? options_v2_intl->local->definitions : NULL;
            if (!us_definitions || !us_definitions[i].key)
                break;

            key = us_definitions[i].key;
            desc = us_definitions[i].desc;
            default_value = us_definitions[i].default_value;
            values = us_definitions[i].values;
            if (local_definitions)
            {
                for (j = 0; local_definitions[j].key; j++)
                {
                    if (strcmp(local_definitions[j].key, us_definitions[i].key) == 0)
                    {
                        if (local_definitions[j].desc)
                            desc = local_definitions[j].desc;
                        local_values = local_definitions[j].values;
                        break;
                    }
                }
            }
        }

        if (!key)
            break;

        OptionListEntryData *data = (OptionListEntryData *)calloc(1, sizeof(OptionListEntryData));
        if (!data)
            continue;
        LinkedListAdd(core_option_list, data);

        data->visibility = 1;

        // printf("Option key: %s\n", key);
        data->key = (char *)malloc(strlen(key) + 1);
        if (data->key)
            strcpy(data->key, key);

        if (desc)
        {
            // printf("Option desc: %s\n", desc);
            data->desc = (char *)malloc(strlen(desc) + 1);
            if (data->desc)
                strcpy(data->desc, desc);
        }

        if (!values)
            continue;

        int n_values = 0;
        while (values[n_values].value)
            n_values++;
        // printf("Option n_values: %d\n", n_values);

        if (n_values == 0)
            continue;

        data->values = (OptionValue *)calloc(n_values, sizeof(OptionValue));
        if (!data->values)
            continue;
        data->n_values = n_values;

        int select_got = 0;

        for (k = 0; k < n_values; k++)
        {
            if (!values[k].value)
                continue;

            // printf("Core option value: value: %s\n", values[k].value);
            if (!select_got && default_value && strcmp(values[k].value, default_value) == 0)
            {
                data->default_select = k;
                data->select = data->default_select;
                select_got = 1;
            }

            data->values[k].value = (char *)malloc(strlen(values[k].value) + 1);
            if (data->values[k].value)
                strcpy(data->values[k].value, values[k].value);

            const char *label = values[k].label;
            if (local_values)
            {
                int l;
                for (l = 0; local_values[l].value; l++)
                {
                    if (strcmp(local_values[l].value, values[k].value) == 0)
                    {
                        if (local_values[l].label)
                            label = local_values[l].label;
                        break;
                    }
                }
            }
            if (label)
            {
                data->values[k].label = (char *)malloc(strlen(label) + 1);
                if (data->values[k].label)
                    strcpy(data->values[k].label, label);
            }
        }
    }

    Setting_SetCoreMenu(core_option_list);
    if (!Emu_IsGameLoading())
        Retro_UpdateCoreOptionsDisplay();

    if (Emu_IsGameLoading() || Emu_IsGameLoaded())
        LoadCoreConfig(TYPE_CONFIG_GAME);
    else
        LoadCoreConfig(TYPE_CONFIG_MAIN);

    return 0;
}
