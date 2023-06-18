#ifndef __M_EMU_DISK_H__
#define __M_EMU_DISK_H__

#include "emu/emu.h"

int Emu_HasDiskControl();

int Emu_DiskSetEjectState(int ejected);
int Emu_DiskGetEjectState();
int Emu_DiskGetImageIndex();
int Emu_DiskSetImageIndex(unsigned index);
int Emu_DiskGetNumImages();
int Emu_DiskReplaceImageIndex(unsigned index, const struct retro_game_info *info);
int Emu_DiskAddImageIndex();

int Emu_DiskSetInitialImage(unsigned index, const char *path);
int Emu_DiskGetImagePath(unsigned index, char *path, size_t len);
int Emu_DiskGetImageLabel(unsigned index, char *label, size_t len);

int Emu_DiskChangeImageIndex(int index);

#endif
