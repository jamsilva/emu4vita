#ifndef __M_SETTING_CONTROL_OPTIONS_H__
#define __M_SETTING_CONTROL_OPTIONS_H__

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Setting/setting.h"
#include "setting_callbacks.h"
#include "Retro/retro.h"
#include "Emu/emu.h"
#include "lang.h"
#include "config.h"

static char key_map_option_texts[16][256];

static OptionMenuItem key_map_option_items[] = {
#if defined(WSC_BUILD)
    {"X1", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT), 0},
    {"X2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP), 0},
    {"X3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT), 0},
    {"X4", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN), 0},
#else
    {"←", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_LEFT), 0},
    {"↑", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_UP), 0},
    {"→", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_RIGHT), 0},
    {"↓", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_DOWN), 0},
#endif
#if defined(FC_BUILD) || defined(GBC_BUILD) || defined(NGP_BUILD)
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"Select", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(GBA_BUILD)
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"L", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"R", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"Select", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(MD_BUILD)
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"C", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"X", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"Y", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {"Z", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {STR_SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(WSC_BUILD)
    {"Y1", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"Y2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {"Y3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"Y4", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(PCE_BUILD)
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"X", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {"Y", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {"L", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"R", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"Select", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
    {STR_SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
#elif defined(FBA_BUILD)
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"C", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {"D", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {"E", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"F", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"G", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {"H", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {STR_COIN, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {STR_START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#elif defined(PS_BUILD)
    {STR_CROSS, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {STR_CIRCLE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {STR_SQUARE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {STR_TRIANGLE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {"L", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"R", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"L2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {"R2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {"L3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3), 0},
    {"R3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3), 0},
    {"Select", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#else
    {"A", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {"B", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {"X", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {"Y", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {"L", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {"R", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {"L2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {"R2", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {"L3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L3), 0},
    {"R3", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R3), 0},
    {"Select", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {"Start", (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
#endif
};

#define STD_KEY_MAP_OPTION(idx, key)                                                                            \
    {                                                                                                           \
        key_map_option_texts[idx], key_map_option_items, sizeof(key_map_option_items) / sizeof(OptionMenuItem), \
            openKeyMapOptionCallback, updateKeyMapOptionCallback, &control_config.key                     \
    }

static void openKeyMapOptionCallback(OptionMenu *option)
{
    uint32_t set_key = *(uint32_t *)(option->userdata);
    int enabled = set_key & ENABLE_KEY_BITMASK;

    OptionMenuItem *item;
    uint32_t map_key;
    int i;
    for (i = 0; i < option->n_items; i++)
    {
        item = &(option->items[i]);
        item->selected = 0;
        if (!enabled)
            continue;

        map_key = (uint32_t)(item->userdata);
        if (set_key & map_key)
            item->selected = 1;
    }
}

static void updateKeyMapOptionCallback(OptionMenu *option)
{
    char *text = option->name;
    uint32_t *set_key = (uint32_t *)(option->userdata);
    int turbo = *set_key & TURBO_KEY_BITMASK;
    uint32_t new_set_key = 0;
    text[0] = '\0';

    OptionMenuItem *item;
    uint32_t map_key;
    int n_maped = 0;
    int i;
    for (i = 0; i < option->n_items; i++)
    {
        item = &(option->items[i]);
        if (item->selected)
        {
            map_key = (uint32_t)(item->userdata);
            new_set_key |= map_key;
            if (n_maped > 0)
                strcat(text, "+");
            strcat(text, item->name);
            n_maped++;
        }
    }
    if (n_maped > 0)
        new_set_key |= ENABLE_KEY_BITMASK;
    else
        strcpy(text, STR_DISABLE);
    if (turbo)
        new_set_key |= TURBO_KEY_BITMASK;
    *set_key = new_set_key;

    control_option_changed = 1;
}

#endif