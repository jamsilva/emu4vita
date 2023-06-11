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

static KeyMapOptionMenuItem key_map_option_items[] = {
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
    {SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
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
    {SWICTH_MODE, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
#elif defined(FBA_BUILD)
    {BUTTON_A, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_B), 0},
    {BUTTON_B, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_A), 0},
    {BUTTON_C, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_Y), 0},
    {BUTTON_D, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_X), 0},
    {BUTTON_E, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L), 0},
    {BUTTON_F, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R), 0},
    {BUTTON_G, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_L2), 0},
    {BUTTON_H, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_R2), 0},
    {COIN, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_SELECT), 0},
    {START, (void *)RETRO_KEY_TO_BITMASK(RETRO_DEVICE_ID_JOYPAD_START), 0},
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

#define STD_KEY_MAP_OPTION(idx, key)                                                                            \
    {                                                                                                           \
        key_map_option_texts[idx], key_map_option_items, sizeof(key_map_option_items) / sizeof(KeyMapOptionMenuItem), \
            openKeyMapOptionCallback, NULL, updateKeyMapOptionCallback, &control_config.key                     \
    }

static void openKeyMapOptionCallback(KeyMapOptionMenu *option)
{
    uint32_t set_key = *(uint32_t *)(option->userdata);
    int enabled = set_key & ENABLE_KEY_BITMASK;

    KeyMapOptionMenuItem *item;
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

static void updateKeyMapOptionCallback(KeyMapOptionMenu *option)
{
    char *text = option->name;
    uint32_t *set_key = (uint32_t *)(option->userdata);
    int turbo = *set_key & TURBO_KEY_BITMASK;
    uint32_t new_set_key = 0;
    text[0] = '\0';

    KeyMapOptionMenuItem *item;
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
            strcat(text, cur_lang[item->lang]);
            n_maped++;
        }
    }
    if (n_maped > 0)
        new_set_key |= ENABLE_KEY_BITMASK;
    else
        strcpy(text, cur_lang[DISABLE]);
    if (turbo)
        new_set_key |= TURBO_KEY_BITMASK;
    *set_key = new_set_key;

    control_option_changed = 1;
}

#endif