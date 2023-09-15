#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/system_param.h>
#include <psp2/ctrl.h>

#include "gui.h"
#include "init.h"
#include "config.h"
#include "utils.h"

Pad old_pad, current_pad, pressed_pad, released_pad, hold_pad, hold2_pad;
Pad hold_count, hold2_count;
static uint64_t disable_psbutton_micros = 0;

void GUI_ReadPad()
{
    SceCtrlData ctrlData;
    int port, i;

    memcpy(old_pad, current_pad, sizeof(Pad));
    memset(current_pad, 0, sizeof(Pad));

    for (i = 0; i < N_CTRL_PORTS; i++)
    {
        if (i > 0)
            port = i + 1;
        else
            port = 0;

        memset(&ctrlData, 0, sizeof(SceCtrlData));
        int ret = sceCtrlPeekBufferPositiveExt2(port, &ctrlData, 1);
        if (ret < 0)
            continue;

        if (ctrlData.buttons & SCE_CTRL_UP)
            current_pad[PAD_UP] = 1;
        if (ctrlData.buttons & SCE_CTRL_DOWN)
            current_pad[PAD_DOWN] = 1;
        if (ctrlData.buttons & SCE_CTRL_LEFT)
            current_pad[PAD_LEFT] = 1;
        if (ctrlData.buttons & SCE_CTRL_RIGHT)
            current_pad[PAD_RIGHT] = 1;
        if (ctrlData.buttons & SCE_CTRL_TRIANGLE)
            current_pad[PAD_TRIANGLE] = 1;
        if (ctrlData.buttons & SCE_CTRL_CIRCLE)
            current_pad[PAD_CIRCLE] = 1;
        if (ctrlData.buttons & SCE_CTRL_CROSS)
            current_pad[PAD_CROSS] = 1;
        if (ctrlData.buttons & SCE_CTRL_SQUARE)
            current_pad[PAD_SQUARE] = 1;
        if (ctrlData.buttons & SCE_CTRL_L1)
            current_pad[PAD_L1] = 1;
        if (ctrlData.buttons & SCE_CTRL_R1)
            current_pad[PAD_R1] = 1;
        if (ctrlData.buttons & SCE_CTRL_L2)
            current_pad[PAD_L2] = 1;
        if (ctrlData.buttons & SCE_CTRL_R2)
            current_pad[PAD_R2] = 1;
        if (ctrlData.buttons & SCE_CTRL_L3)
            current_pad[PAD_L3] = 1;
        if (ctrlData.buttons & SCE_CTRL_R3)
            current_pad[PAD_R3] = 1;
        if (ctrlData.buttons & SCE_CTRL_START)
            current_pad[PAD_START] = 1;
        if (ctrlData.buttons & SCE_CTRL_SELECT)
            current_pad[PAD_SELECT] = 1;
        if (ctrlData.buttons & SCE_CTRL_PSBUTTON)
            current_pad[PAD_PSBUTTON] = 1;

        if (ctrlData.lx < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_LEFT] = 1;
        else if (ctrlData.lx > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_RIGHT] = 1;

        if (ctrlData.ly < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_UP] = 1;
        else if (ctrlData.ly > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_LEFT_ANALOG_DOWN] = 1;

        if (ctrlData.rx < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_LEFT] = 1;
        else if (ctrlData.rx > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_RIGHT] = 1;

        if (ctrlData.ry < ANALOG_CENTER - ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_UP] = 1;
        else if (ctrlData.ry > ANALOG_CENTER + ANALOG_THRESHOLD)
            current_pad[PAD_RIGHT_ANALOG_DOWN] = 1;
    }

    for (i = 0; i < PAD_N_BUTTONS; i++)
    {
        pressed_pad[i] = current_pad[i] & ~old_pad[i];
        released_pad[i] = ~current_pad[i] & old_pad[i];

        hold_pad[i] = pressed_pad[i];
        hold2_pad[i] = pressed_pad[i];

        if (current_pad[i])
        {
            if (hold_count[i] >= 10)
            {
                hold_pad[i] = 1;
                hold_count[i] = 6;
            }

            if (hold2_count[i] >= 10)
            {
                hold2_pad[i] = 1;
                hold2_count[i] = 10;
            }

            hold_count[i]++;
            hold2_count[i]++;
        }
        else
        {
            hold_count[i] = 0;
            hold2_count[i] = 0;
        }
    }

    if (enter_button == SCE_SYSTEM_PARAM_ENTER_BUTTON_CIRCLE)
    {
        old_pad[PAD_ENTER] = old_pad[PAD_CIRCLE];
        current_pad[PAD_ENTER] = current_pad[PAD_CIRCLE];
        pressed_pad[PAD_ENTER] = pressed_pad[PAD_CIRCLE];
        released_pad[PAD_ENTER] = released_pad[PAD_CIRCLE];
        hold_pad[PAD_ENTER] = hold_pad[PAD_CIRCLE];
        hold2_pad[PAD_ENTER] = hold2_pad[PAD_CIRCLE];

        old_pad[PAD_CANCEL] = old_pad[PAD_CROSS];
        current_pad[PAD_CANCEL] = current_pad[PAD_CROSS];
        pressed_pad[PAD_CANCEL] = pressed_pad[PAD_CROSS];
        released_pad[PAD_CANCEL] = released_pad[PAD_CROSS];
        hold_pad[PAD_CANCEL] = hold_pad[PAD_CROSS];
        hold2_pad[PAD_CANCEL] = hold2_pad[PAD_CROSS];
    }
    else
    {
        old_pad[PAD_ENTER] = old_pad[PAD_CROSS];
        current_pad[PAD_ENTER] = current_pad[PAD_CROSS];
        pressed_pad[PAD_ENTER] = pressed_pad[PAD_CROSS];
        released_pad[PAD_ENTER] = released_pad[PAD_CROSS];
        hold_pad[PAD_ENTER] = hold_pad[PAD_CROSS];
        hold2_pad[PAD_ENTER] = hold2_pad[PAD_CROSS];

        old_pad[PAD_CANCEL] = old_pad[PAD_CIRCLE];
        current_pad[PAD_CANCEL] = current_pad[PAD_CIRCLE];
        pressed_pad[PAD_CANCEL] = pressed_pad[PAD_CIRCLE];
        released_pad[PAD_CANCEL] = released_pad[PAD_CIRCLE];
        hold_pad[PAD_CANCEL] = hold_pad[PAD_CIRCLE];
        hold2_pad[PAD_CANCEL] = hold2_pad[PAD_CIRCLE];
    }

    if (current_pad[PAD_PSBUTTON])
    {
        if (!old_pad[PAD_PSBUTTON])
            disable_psbutton_micros = sceKernelGetProcessTimeWide() + DISABLE_PSBUTTON_EVENT_HOLD_MICROS;
        else if (sceKernelGetProcessTimeWide() >= disable_psbutton_micros)
            SetPSbuttonEventEnabled(0);
    }
    else
    {
        if (!old_pad[PAD_PSBUTTON])
            SetPSbuttonEventEnabled(1);
    }
}

void GUI_CleanPad()
{
    memset(&old_pad, 0, sizeof(Pad));
    memset(&current_pad, 0, sizeof(Pad));
    memset(&pressed_pad, 0, sizeof(Pad));
    memset(&released_pad, 0, sizeof(Pad));
    memset(&hold_pad, 0, sizeof(Pad));
    memset(&hold2_pad, 0, sizeof(Pad));
}
