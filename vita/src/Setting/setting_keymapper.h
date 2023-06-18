#ifndef __M_SETTING_KEYMAPPING_H__
#define __M_SETTING_KEYMAPPING_H__

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "setting/setting.h"
#include "setting/setting_variables.h"
#include "emu/emu.h"
#include "lang.h"
#include "config.h"

static char emu_key_mapper_option_texts[16][256];
static char hot_key_mapper_option_texts[16][256];

static KeyMapperOptionMenuItem emu_key_mapper_option_items[] = {
#if defined(WSC_BUILD)
    {BUTTON_X1, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT), 0},
    {BUTTON_X2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP), 0},
    {BUTTON_X3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT), 0},
    {BUTTON_X4, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN), 0},
#else
    {BUTTON_LEFT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT), 0},
    {BUTTON_UP, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP), 0},
    {BUTTON_RIGHT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT), 0},
    {BUTTON_DOWN, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN), 0},
#endif
#if defined(FC_BUILD) || defined(GBC_BUILD) || defined(NGP_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_SELECT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(GBA_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_L, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_R, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_SELECT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(MD_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_C, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_X, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_Y, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_Z, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {LABEL_SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(WSC_BUILD)
    {BUTTON_Y1, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_Y2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {BUTTON_Y3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_Y4, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(PCE_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_X, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_Y, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_L, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_R, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_SELECT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
    {LABEL_SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
#elif defined(FBA_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_C, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_D, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_E, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_F, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_G, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {BUTTON_H, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {LABEL_COIN, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {LABEL_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(PS_BUILD)
    {BUTTON_CROSS, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_CIRCLE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_SQUARE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_TRIANGLE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_L, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_R, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_L2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {BUTTON_R2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {BUTTON_L3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3), 0},
    {BUTTON_R3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3), 0},
    {BUTTON_SELECT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#else
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_X, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_Y, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_L, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_R, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_L2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {BUTTON_R2, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {BUTTON_L3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3), 0},
    {BUTTON_R3, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3), 0},
    {BUTTON_SELECT, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {BUTTON_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#endif
};

#define STD_EMU_KEY_MAPPER_OPTION(string_idx, config_element)                                                                                        \
    {                                                                                                                                                \
        emu_key_mapper_option_texts[string_idx], emu_key_mapper_option_items, sizeof(emu_key_mapper_option_items) / sizeof(KeyMapperOptionMenuItem), \
            openKeyMapperOptionCallback, NULL, updateKeyMapperOptionCallback, &control_config.config_element                                         \
    }

static KeyMapperOptionMenuItem hot_key_mapper_option_items[] = {
    {BUTTON_PSBUTTON, (void *)SCE_CTRL_PSBUTTON, 0},
    {BUTTON_LEFT, (void *)SCE_CTRL_LEFT, 0},
    {BUTTON_UP, (void *)SCE_CTRL_UP, 0},
    {BUTTON_RIGHT, (void *)SCE_CTRL_RIGHT, 0},
    {BUTTON_DOWN, (void *)SCE_CTRL_DOWN, 0},
    {BUTTON_CROSS, (void *)SCE_CTRL_CROSS, 0},
    {BUTTON_CIRCLE, (void *)SCE_CTRL_CIRCLE, 0},
    {BUTTON_SQUARE, (void *)SCE_CTRL_SQUARE, 0},
    {BUTTON_TRIANGLE, (void *)SCE_CTRL_TRIANGLE, 0},
    {BUTTON_L, (void *)SCE_CTRL_L1, 0},
    {BUTTON_R, (void *)SCE_CTRL_R1, 0},
    {BUTTON_L2, (void *)SCE_CTRL_L2, 0},
    {BUTTON_R2, (void *)SCE_CTRL_R2, 0},
    {BUTTON_L3, (void *)SCE_CTRL_L3, 0},
    {BUTTON_R3, (void *)SCE_CTRL_R3, 0},
    {BUTTON_SELECT, (void *)SCE_CTRL_SELECT, 0},
    {BUTTON_START, (void *)SCE_CTRL_START, 0},
    {BUTTON_LEFT_ANALOG_LEFT, (void *)EXT_CTRL_LEFT_ANLOG_LEFT, 0},
    {BUTTON_LEFT_ANALOG_UP, (void *)EXT_CTRL_LEFT_ANLOG_UP, 0},
    {BUTTON_LEFT_ANALOG_RIGHT, (void *)EXT_CTRL_LEFT_ANLOG_RIGHT, 0},
    {BUTTON_LEFT_ANALOG_DOWN, (void *)EXT_CTRL_LEFT_ANLOG_DOWN, 0},
    {BUTTON_RIGHT_ANALOG_LEFT, (void *)EXT_CTRL_RIGHT_ANLOG_LEFT, 0},
    {BUTTON_RIGHT_ANALOG_UP, (void *)EXT_CTRL_RIGHT_ANLOG_UP, 0},
    {BUTTON_RIGHT_ANALOG_RIGHT, (void *)EXT_CTRL_RIGHT_ANLOG_RIGHT, 0},
    {BUTTON_RIGHT_ANALOG_DOWN, (void *)EXT_CTRL_RIGHT_ANLOG_DOWN, 0},
};

#define STD_HOT_KEY_MAPPER_OPTION(string_idx, config_element)                                                                                        \
    {                                                                                                                                                \
        hot_key_mapper_option_texts[string_idx], hot_key_mapper_option_items, sizeof(hot_key_mapper_option_items) / sizeof(KeyMapperOptionMenuItem), \
            openKeyMapperOptionCallback, NULL, updateKeyMapperOptionCallback, &misc_config.config_element                                            \
    }

static void openKeyMapperOptionCallback(KeyMapperOptionMenu *option)
{
    uint32_t config_key = *(uint32_t *)(option->userdata);
    int enabled = config_key & ENABLE_KEY_BITMASK;

    KeyMapperOptionMenuItem *item;
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

static void updateKeyMapperOptionCallback(KeyMapperOptionMenu *option)
{
    char *text = option->name;
    uint32_t *config_key = (uint32_t *)(option->userdata);
    int turbo = *config_key & TURBO_KEY_BITMASK;
    uint32_t new_config_key = 0;
    text[0] = '\0';

    KeyMapperOptionMenuItem *item;
    uint32_t mapping_key;
    int n_mapped = 0;
    int i;
    for (i = 0; i < option->n_items; i++)
    {
        item = &(option->items[i]);
        if (item->selected)
        {
            mapping_key = (uint32_t)(item->userdata);
            new_config_key |= mapping_key;
            if (n_mapped > 0)
                strcat(text, "+");
            strcat(text, cur_lang[item->lang]);
            n_mapped++;
        }
    }
    if (n_mapped > 0)
        new_config_key |= ENABLE_KEY_BITMASK;
    else
        strcpy(text, cur_lang[DISABLE]);
    if (turbo)
        new_config_key |= TURBO_KEY_BITMASK;
    *config_key = new_config_key;

    control_option_changed = 1;
}

#endif