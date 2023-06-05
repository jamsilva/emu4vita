#ifndef __M_EMU_VIDEO_H__
#define __M_EMU_VIDEO_H__

#include <stdint.h>
#include "Gui/gui.h"

void Emu_PauseVideo();
void Emu_ResumeVideo();
void Emu_RequestRefreshVideo();

void Emu_DrawVideo();

int Emu_InitVideo();
int Emu_DeinitVideo();

GUI_Texture *Emu_GetVideoTexture();

void Emu_ReshowCtrlPlayer();

void Emu_MakeVideoBaseWH(uint32_t *res_width, uint32_t *res_height);
void Emu_MakeVideoDisplayWH(uint32_t *res_width, uint32_t *res_height, int rotate);

uint32_t *Emu_GetVideoScreenshotData(uint32_t *width, uint32_t *height, uint64_t *size, int rotate);
int Emu_SaveVideoScreenshot(char *path);

#endif