#ifndef __M_EMU_INPUT_H__
#define __M_EMU_INPUT_H__

#include <stdint.h>

typedef struct
{
    uint32_t *value;
    int local_key;
    uint8_t old_presseds[N_CTRL_PORTS];
    uint32_t hold_counts[N_CTRL_PORTS];
} EmuKeyOption;

typedef struct
{
    uint32_t *value;
    void (*callback)();
    uint8_t old_presseds[N_CTRL_PORTS];
} HotKeyOption;

void Emu_PollInput();
int Emu_InitInput();
int Emu_DeinitInput();

#endif