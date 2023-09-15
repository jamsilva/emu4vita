#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "emu/emu.h"
#include "setting.h"
#include "lang.h"
#include "config.h"

#ifndef __SETTING_MENU_EMBED
#include "setting_variables.c"
#endif

static CheckBoxOptionMenuItem emu_key_mapper_option_items[] = {
#if defined(WSC_BUILD)
    {{BUTTON_X1, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT)},
    {{BUTTON_X2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP)},
    {{BUTTON_X3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT)},
    {{BUTTON_X4, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN)},
#else
    {{BUTTON_LEFT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT)},
    {{BUTTON_UP, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP)},
    {{BUTTON_RIGHT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT)},
    {{BUTTON_DOWN, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN)},
#endif
#if defined(FC_BUILD) || defined(GBC_BUILD) || defined(NGP_BUILD)
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_SELECT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#elif defined(GBA_BUILD)
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_L, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_R, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_SELECT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#elif defined(MD_BUILD)
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_C, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_X, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_Y, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X)},
    {{BUTTON_Z, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{LABEL_SWICTH_MODE, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#elif defined(WSC_BUILD)
    {{BUTTON_Y1, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_Y2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2)},
    {{BUTTON_Y3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_Y4, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2)},
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#elif defined(PCE_BUILD)
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_X, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X)},
    {{BUTTON_Y, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y)},
    {{BUTTON_L, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_R, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_SELECT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
    {{LABEL_SWICTH_MODE, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2)},
#elif defined(FBA_BUILD)
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_C, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y)},
    {{BUTTON_D, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X)},
    {{BUTTON_E, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_F, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_G, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2)},
    {{BUTTON_H, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2)},
    {{LABEL_COIN, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{LABEL_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#elif defined(PS_BUILD)
    {{BUTTON_CROSS, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_CIRCLE, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_SQUARE, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y)},
    {{BUTTON_TRIANGLE, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X)},
    {{BUTTON_L, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_R, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_L2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2)},
    {{BUTTON_R2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2)},
    {{BUTTON_L3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3)},
    {{BUTTON_R3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3)},
    {{BUTTON_SELECT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#else
    {{BUTTON_A, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A)},
    {{BUTTON_B, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B)},
    {{BUTTON_X, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X)},
    {{BUTTON_Y, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y)},
    {{BUTTON_L, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L)},
    {{BUTTON_R, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R)},
    {{BUTTON_L2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2)},
    {{BUTTON_R2, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2)},
    {{BUTTON_L3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3)},
    {{BUTTON_R3, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3)},
    {{BUTTON_SELECT, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT)},
    {{BUTTON_START, NULL}, 0, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START)},
#endif
};

#define STD_EMU_KEY_MAPPER_OPTION(text_idx, config_element)                                                                   \
    {                                                                                                                         \
        {LANG_NULL, NULL}, emu_key_mapper_option_items, sizeof(emu_key_mapper_option_items) / sizeof(CheckBoxOptionMenuItem), \
            openKeyMapperOptionCallback, NULL, controlKeymapperOptionChangedCallback, &control_config.config_element          \
    }

static CheckBoxOptionMenuItem hot_key_mapper_option_items[] = {
    {{BUTTON_PSBUTTON, NULL}, 0, (void *)SCE_CTRL_PSBUTTON},
    {{BUTTON_LEFT, NULL}, 0, (void *)SCE_CTRL_LEFT},
    {{BUTTON_UP, NULL}, 0, (void *)SCE_CTRL_UP},
    {{BUTTON_RIGHT, NULL}, 0, (void *)SCE_CTRL_RIGHT},
    {{BUTTON_DOWN, NULL}, 0, (void *)SCE_CTRL_DOWN},
    {{BUTTON_CROSS, NULL}, 0, (void *)SCE_CTRL_CROSS},
    {{BUTTON_CIRCLE, NULL}, 0, (void *)SCE_CTRL_CIRCLE},
    {{BUTTON_SQUARE, NULL}, 0, (void *)SCE_CTRL_SQUARE},
    {{BUTTON_TRIANGLE, NULL}, 0, (void *)SCE_CTRL_TRIANGLE},
    {{BUTTON_L, NULL}, 0, (void *)SCE_CTRL_L1},
    {{BUTTON_R, NULL}, 0, (void *)SCE_CTRL_R1},
    {{BUTTON_L2, NULL}, 0, (void *)SCE_CTRL_L2},
    {{BUTTON_R2, NULL}, 0, (void *)SCE_CTRL_R2},
    {{BUTTON_L3, NULL}, 0, (void *)SCE_CTRL_L3},
    {{BUTTON_R3, NULL}, 0, (void *)SCE_CTRL_R3},
    {{BUTTON_SELECT, NULL}, 0, (void *)SCE_CTRL_SELECT},
    {{BUTTON_START, NULL}, 0, (void *)SCE_CTRL_START},
    {{BUTTON_LEFT_ANALOG_LEFT, NULL}, 0, (void *)EXT_CTRL_LEFT_ANLOG_LEFT},
    {{BUTTON_LEFT_ANALOG_UP, NULL}, 0, (void *)EXT_CTRL_LEFT_ANLOG_UP},
    {{BUTTON_LEFT_ANALOG_RIGHT, NULL}, 0, (void *)EXT_CTRL_LEFT_ANLOG_RIGHT},
    {{BUTTON_LEFT_ANALOG_DOWN, NULL}, 0, (void *)EXT_CTRL_LEFT_ANLOG_DOWN},
    {{BUTTON_RIGHT_ANALOG_LEFT, NULL}, 0, (void *)EXT_CTRL_RIGHT_ANLOG_LEFT},
    {{BUTTON_RIGHT_ANALOG_UP, NULL}, 0, (void *)EXT_CTRL_RIGHT_ANLOG_UP},
    {{BUTTON_RIGHT_ANALOG_RIGHT, NULL}, 0, (void *)EXT_CTRL_RIGHT_ANLOG_RIGHT},
    {{BUTTON_RIGHT_ANALOG_DOWN, NULL}, 0, (void *)EXT_CTRL_RIGHT_ANLOG_DOWN},
};

#define STD_HOT_KEY_MAPPER_OPTION(text_idx, config_element)                                                                   \
    {                                                                                                                         \
        {LANG_NULL, NULL}, hot_key_mapper_option_items, sizeof(hot_key_mapper_option_items) / sizeof(CheckBoxOptionMenuItem), \
            openKeyMapperOptionCallback, NULL, hotkeyOptionUpdateCallback, &hotkey_config.config_element                      \
    }

static void openKeyMapperOptionCallback(CheckBoxOptionMenu *option)
{
    uint32_t config_key = *(uint32_t *)(option->userdata);
    int enabled = config_key & ENABLE_KEY_BITMASK;

    CheckBoxOptionMenuItem *item;
    uint32_t mapping_key;
    int i;
    for (i = 0; i < option->n_items; i++)
    {
        item = &(option->items[i]);
        item->selected = 0;
        if (!enabled)
            continue;

        mapping_key = (uint32_t)(item->userdata);
        if (config_key & mapping_key)
            item->selected = 1;
    }
}

static void updateKeyMapperOptionCallback(CheckBoxOptionMenu *option)
{
    if (!option->name.string)
        option->name.string = (char *)malloc(256);
    if (option->name.string)
        option->name.string[0] = '\0';

    uint32_t *config_key = (uint32_t *)(option->userdata);
    int turbo = *config_key & TURBO_KEY_BITMASK;
    uint32_t new_config_key = 0;

    int n_mapped = 0;
    int i;
    for (i = 0; i < option->n_items; i++)
    {
        CheckBoxOptionMenuItem *option_item = &(option->items[i]);
        if (option_item->selected)
        {
            uint32_t mapping_key = (uint32_t)(option_item->userdata);
            new_config_key |= mapping_key;
            if (option->name.string)
            {
                char *name = GetLangString(&option_item->name);
                if (name)
                {
                    if (n_mapped > 0)
                        strcat(option->name.string, "+");
                    strcat(option->name.string, name);
                }
            }
            n_mapped++;
        }
    }

    if (n_mapped > 0)
    {
        new_config_key |= ENABLE_KEY_BITMASK;
        option->name.lang = LANG_NULL;
    }
    else
    {
        option->name.lang = DISABLE;
    }

    if (turbo)
        new_config_key |= TURBO_KEY_BITMASK;

    *config_key = new_config_key;
}
