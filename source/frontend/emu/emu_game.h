#ifndef __M_EMU_GAME_H__
#define __M_EMU_GAME_H__

#include "file.h"

enum TypeGameRunAction
{
    TYPE_GAME_RUN_EVENT_ACTION_NONE,
    TYPE_GAME_RUN_EVENT_ACTION_SAVE_STATE,
    TYPE_GAME_RUN_EVENT_ACTION_LOAD_STATE,
    TYPE_GAME_RUN_EVENT_ACTION_RESET,
    TYPE_GAME_RUN_EVENT_ACTION_EXIT,
};

typedef struct EmuGameInfo
{
    char path[MAX_PATH_LENGTH];
    int state_num;
} EmuGameInfo;

int Emu_IsGameRun();
int Emu_IsGameLoading();
int Emu_IsGameLoaded();
double Emu_GetCurrentFps();
float Emu_GetCurrentRunSpeed();
void Emu_SetRunSpeed(float speed);

int Emu_StartGame(EmuGameInfo *info);
void Emu_ExitGame();
void Emu_RunGame();
void Emu_PauseGame();
void Emu_ResumeGame();
void Emu_ResetGame();
int Emu_ReloadGame();
void Emu_SpeedUpGame();
void Emu_SpeedDownGame();

void Emu_SetGameRunEventAction(int type);

#endif