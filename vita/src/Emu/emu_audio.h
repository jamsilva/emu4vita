#ifndef __M_EMU_AUDIO_H__
#define __M_EMU_AUDIO_H__

#include <stdint.h>

void Emu_CleanAudioSound();

void Emu_PauseAudio();
void Emu_ResumeAudio();

int Emu_SetAudioVolume(int left, int right);
int Emu_SetAudioConfig(int samples, uint16_t freq, int stereo);
int Emu_UpdateAudioSampleRate(uint32_t sample_rate);

int Emu_InitAudio();
int Emu_DeinitAudio();


#endif
