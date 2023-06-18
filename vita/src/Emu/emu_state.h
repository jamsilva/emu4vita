#ifndef __M_EMU_STATE_H__
#define __M_EMU_STATE_H__

#include "gui/gui.h"

#define STATES_VERSION 1

typedef struct
{
    uint32_t version;           // 0x00
    uint64_t screenshot_offset; // 0x04
    uint32_t screenshot_width;  // 0x0C
    uint32_t screenshot_height; // 0x10
    uint64_t screenshot_size;   // 0x14
    uint64_t preview_offset;    // 0x1C
    uint32_t preview_width;     // 0x24
    uint32_t preview_height;    // 0x28
    uint64_t preview_size;      // 0x2C
    uint64_t state_offset;      // 0x34
    uint64_t state_size;        // 0x3C
    uint32_t disk_index;        // 0x44
    char reserved[0x20];        // 0x48
} EmuStateHeader;               // 0x68

void MakeSavestateDir(char *path);
void MakeSavestatePath(char *path, int num);

GUI_Texture *Emu_GetStateScreenshotTexture(int num);
GUI_Texture *Emu_GetStatePreviewTexture(int num);

int Emu_LoadState(int num);
int Emu_SaveState(int num);
int Emu_DeleteState(int num);

#endif