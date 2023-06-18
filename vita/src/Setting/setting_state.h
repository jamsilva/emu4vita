#ifndef __M_STATES_H__
#define __M_STATES_H__

void Setting_RefreshStateLayout();

int Setting_InitState();
int Setting_DeinitState();

void Setting_DrawState();
void Setting_CtrlState();

void Setting_SetStateSelectId(int id);
int Setting_GetStateSelectId();

int Setting_GetStatePreviewWidth();
int Setting_GetStatePreviewHeight();

int Setting_LoadState(int num);
int Setting_SaveState(int num);
int Setting_DeleteState(int num);

#endif