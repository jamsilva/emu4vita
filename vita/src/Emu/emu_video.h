#ifndef __M_EMU_VIDEO_H__
#define __M_EMU_VIDEO_H__

#include <stdint.h>
#include "gui/gui.h"

void Emu_SetMicrosPerFrame(uint64_t micros);

void Emu_PauseVideo();
void Emu_ResumeVideo();
void Emu_PushUpdateVideoDisplay();

void Emu_DrawVideo();

int Emu_InitVideo();
int Emu_DeinitVideo();

GUI_Texture *Emu_GetVideoTexture();
GUI_Texture *Emu_UpdateVideoTexture(int width, int height);

void Emu_ReshowCtrlPlayer();

int Emu_GetVideoDisplayRotate();
void Emu_GetVideoBaseWH(uint32_t *width, uint32_t *height);
void Emu_GetVideoDisplayWH(uint32_t *width, uint32_t *height);

uint32_t *Emu_GetVideoScreenshotData(uint32_t *width, uint32_t *height, uint64_t *size, int rotate);
int Emu_SaveVideoScreenshot(char *path);

#endif