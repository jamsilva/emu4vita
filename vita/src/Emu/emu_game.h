#ifndef __M_EMU_GAME_H__
#define __M_EMU_GAME_H__

int Emu_IsGameRun();
int Emu_IsGameLoading();
int Emu_IsGameLoaded();
double Emu_GetCurrentFps();
float Emu_GetCurrentRunSpeed();
void Emu_SetRunSpeed(float speed);

int Emu_LoadGame(const char *path);
void Emu_ExitGame();
void Emu_RunGame();
void Emu_PauseGame();
void Emu_ResumeGame();
void Emu_ResetGame();
int Emu_ReloadGame();
void Emu_SpeedUpGame();
void Emu_SpeedDownGame();

void Emu_RequestResetGame();
void Emu_RequestExitGame();
void Emu_RequestSaveState();
void Emu_RequestLoadState();

#endif