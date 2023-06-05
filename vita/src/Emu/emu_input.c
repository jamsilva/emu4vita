#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/ctrl.h>
#include <psp2/touch.h>

#include "Setting/setting.h"
#include "Retro/retro.h"
#include "Gui/gui.h"
#include "emu.h"
#include "utils.h"
#include "config.h"
#include "init.h"

typedef struct
{
    int map_key;
    int sx;
    int sy;
    int dx;
    int dy;
} TouchMap;

typedef struct
{
    uint32_t *enable;
    TouchMap *maps;
    int n_maps;
} TouchMapOption;

#define RETRO_PAD_N_BUTTONS 16

#define FRONT_TOUCH_WIDTH 200
#define FRONT_TOUCH_HEIGHT 200
#define BACK_TOUCH_WIDTH 400
#define BACK_TOUCH_HEIGHT 300
#define BACK_TOUCH_PADDING_L 0

extern GUI_Dialog setting_dialog;

static void changeMapPortUp();
static void changeMapPortDown();

TouchMap front_touch_maps[] = {
    {SCE_CTRL_L2, 0, 0, FRONT_TOUCH_WIDTH, FRONT_TOUCH_HEIGHT},
    {SCE_CTRL_R2, GUI_SCREEN_WIDTH - FRONT_TOUCH_WIDTH, 0, GUI_SCREEN_WIDTH, FRONT_TOUCH_HEIGHT},
    {SCE_CTRL_L3, 0, GUI_SCREEN_HEIGHT - FRONT_TOUCH_HEIGHT, FRONT_TOUCH_WIDTH, GUI_SCREEN_HEIGHT},
    {SCE_CTRL_R3, GUI_SCREEN_WIDTH - FRONT_TOUCH_WIDTH, GUI_SCREEN_HEIGHT - FRONT_TOUCH_HEIGHT, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT},
};

TouchMap back_touch_maps[] = {
    {SCE_CTRL_L2, BACK_TOUCH_PADDING_L, 0, BACK_TOUCH_PADDING_L + BACK_TOUCH_WIDTH, BACK_TOUCH_HEIGHT},
    {SCE_CTRL_R2, GUI_SCREEN_WIDTH - BACK_TOUCH_PADDING_L - BACK_TOUCH_WIDTH, 0, GUI_SCREEN_WIDTH, BACK_TOUCH_HEIGHT},
    {SCE_CTRL_L3, BACK_TOUCH_PADDING_L, GUI_SCREEN_HEIGHT - BACK_TOUCH_HEIGHT, BACK_TOUCH_PADDING_L + BACK_TOUCH_WIDTH, GUI_SCREEN_HEIGHT},
    {SCE_CTRL_R3, GUI_SCREEN_WIDTH - BACK_TOUCH_PADDING_L - BACK_TOUCH_WIDTH, GUI_SCREEN_HEIGHT - BACK_TOUCH_HEIGHT, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT},
};

TouchMapOption touch_map_options[2] = {
    {&control_config.front_touch_pad, front_touch_maps, sizeof(front_touch_maps) / sizeof(TouchMap)},
    {&control_config.back_touch_pad, back_touch_maps, sizeof(back_touch_maps) / sizeof(TouchMap)},
};

KeyMapOption key_map_options[] = {
    {&control_config.button_left, SCE_CTRL_LEFT, {0}, {0}},
    {&control_config.button_up, SCE_CTRL_UP, {0}, {0}},
    {&control_config.button_right, SCE_CTRL_RIGHT, {0}, {0}},
    {&control_config.button_down, SCE_CTRL_DOWN, {0}, {0}},
    {&control_config.button_cross, SCE_CTRL_CROSS, {0}, {0}},
    {&control_config.button_circle, SCE_CTRL_CIRCLE, {0}, {0}},
    {&control_config.button_square, SCE_CTRL_SQUARE, {0}, {0}},
    {&control_config.button_triangle, SCE_CTRL_TRIANGLE, {0}, {0}},
    {&control_config.button_select, SCE_CTRL_SELECT, {0}, {0}},
    {&control_config.button_start, SCE_CTRL_START, {0}, {0}},
    {&control_config.button_l, SCE_CTRL_L1, {0}, {0}},
    {&control_config.button_r, SCE_CTRL_R1, {0}, {0}},
    {&control_config.button_l2, SCE_CTRL_L2, {0}, {0}},
    {&control_config.button_r2, SCE_CTRL_R2, {0}, {0}},
    {&control_config.button_l3, SCE_CTRL_L3, {0}, {0}},
    {&control_config.button_r3, SCE_CTRL_R3, {0}, {0}},
};
#define N_KEY_MAP_OPTIONS 16 // (sizeof(key_map_options) / sizeof(KeyMapOption))

HotKeyOption hot_key_options[] = {
    {SCE_CTRL_PSBUTTON | SCE_CTRL_SQUARE, Emu_RequestSaveState, {0}},
    {SCE_CTRL_PSBUTTON | SCE_CTRL_TRIANGLE, Emu_RequestLoadState, {0}},
    {SCE_CTRL_PSBUTTON | SCE_CTRL_CROSS, Emu_RequestExitGame, {0}},
    {SCE_CTRL_PSBUTTON | SCE_CTRL_L1, Emu_SpeedDownGame, {0}},
    {SCE_CTRL_PSBUTTON | SCE_CTRL_R1, Emu_SpeedUpGame, {0}},
    {SCE_CTRL_PSBUTTON | EXT_CTRL_RIGHT_ANLOG_LEFT, changeMapPortDown, {0}},
    {SCE_CTRL_PSBUTTON | EXT_CTRL_RIGHT_ANLOG_RIGHT, changeMapPortUp, {0}},
};
#define N_HOT_KEY_OPTIONS 7 // (sizeof(hot_key_options) / sizeof(HotKeyOption))

static uint8_t psbutton_old_pressed[N_CTRL_PORTS];
static uint32_t psbutton_hold_count[N_CTRL_PORTS];
static uint32_t retro_keys[N_CTRL_PORTS];

static void changeMapPortUp()
{
    if (control_config.map_port < N_CTRL_PORTS - 1)
        control_config.map_port++;
    else
        control_config.map_port = 0;
    SaveControlConfig(CONFIG_TYPE_GAME);
    Emu_ReshowCtrlPlayer();
}

static void changeMapPortDown()
{
    if (control_config.map_port > 0)
        control_config.map_port--;
    else
        control_config.map_port = N_CTRL_PORTS - 1;
    SaveControlConfig(CONFIG_TYPE_GAME);
    Emu_ReshowCtrlPlayer();
}

static void cleanInputKeys()
{
    int i, j;

    for (i = 0; i < N_KEY_MAP_OPTIONS; i++)
    {
        for (j = 0; j < N_CTRL_PORTS; j++)
        {
            key_map_options[i].old_presseds[j] = 0;
            key_map_options[i].hold_counts[j] = 0;
        }
    }

    for (i = 0; i < N_HOT_KEY_OPTIONS; i++)
    {
        for (j = 0; j < N_CTRL_PORTS; j++)
        {
            hot_key_options[i].old_presseds[j] = 0;
        }
    }

    for (i = 0; i < N_CTRL_PORTS; i++)
    {
        psbutton_old_pressed[i] = 0;
        psbutton_hold_count[i] = 0;
    }
}

static void TouchToButtons(uint32_t *buttons)
{
    if (is_vitatv_model)
        return;

    SceTouchData touch_data;
    int i, j, k;
    int x, y;
    for (i = 0; i < 2; i++)
    {
        TouchMapOption *option = &touch_map_options[i];
        if (!*(option->enable))
            continue;

        memset(&touch_data, 0, sizeof(SceTouchData));
        if (sceTouchPeek(i, &touch_data, 1) < 0)
            continue;

        TouchMap *maps = option->maps;
        for (j = 0; j < touch_data.reportNum; j++)
        {
            x = lerp(touch_data.report[j].x, 1919, GUI_SCREEN_WIDTH);
            y = lerp(touch_data.report[j].y, 1087, GUI_SCREEN_HEIGHT);
            for (k = 0; k < option->n_maps; k++)
            {
                if ((x >= maps[k].sx) && (x <= maps[k].dx) && (y >= maps[k].sy) && (y <= maps[k].dy))
                    *buttons |= maps->map_key;
            }
        }
    }
}

static void AnalogToButtons(uint8_t lanalog_x, uint8_t lanalog_y, uint8_t ranalog_x, uint8_t ranalog_y, uint32_t *buttons)
{
    if (lanalog_x < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_LEFT_ANLOG_LEFT;
    else if (lanalog_x > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_LEFT_ANLOG_RIGHT;
    if (lanalog_y < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_LEFT_ANLOG_UP;
    else if (lanalog_y > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_LEFT_ANLOG_DOWN;

    if (ranalog_x < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_RIGHT_ANLOG_LEFT;
    else if (ranalog_x > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_RIGHT_ANLOG_RIGHT;
    if (ranalog_y < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_RIGHT_ANLOG_UP;
    else if (ranalog_y > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= EXT_CTRL_RIGHT_ANLOG_DOWN;
}

static void AnalogToDpads(uint8_t analog_x, uint8_t analog_y, uint32_t *buttons)
{
    if (analog_x < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= SCE_CTRL_LEFT;
    else if (analog_x > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= SCE_CTRL_RIGHT;
    if (analog_y < ANALOG_CENTER - ANALOG_THRESHOLD)
        *buttons |= SCE_CTRL_UP;
    else if (analog_y > ANALOG_CENTER + ANALOG_THRESHOLD)
        *buttons |= SCE_CTRL_DOWN;
}

static void LocalKeyToEmuKey(KeyMapOption *option, int local_port, int map_port, uint32_t buttons)
{
    uint8_t cur_pressed = ((buttons & option->local_key) != 0);
    uint8_t old_pressed = option->old_presseds[local_port];
    option->old_presseds[local_port] = cur_pressed;

    if (cur_pressed)
    {
        option->hold_counts[local_port]++;

        uint32_t value = *(option->value);
        int key_enabled = value & ENABLE_KEY_BITMASK;
        if (!key_enabled)
            return;

        int trubo_enabled = value & TURBO_KEY_BITMASK;
        int hold_count = option->hold_counts[local_port];

        if (!trubo_enabled || !old_pressed || (hold_count % (control_config.turbo_delay + 1) == 0))
        {
            value &= 0x00FFFFFF;
            retro_keys[map_port] |= value;
        }
    }
    else
    {
        option->hold_counts[local_port] = 0;
    }
}

static int checkHotKey(int port, uint32_t buttons)
{
    HotKeyOption *option;
    uint8_t cur_pressed;
    uint8_t old_pressed;

    int i;
    for (i = 0; i < N_HOT_KEY_OPTIONS; i++)
    {
        option = &hot_key_options[i];
        cur_pressed = ((buttons & option->key) == option->key);
        old_pressed = option->old_presseds[port];
        option->old_presseds[port] = cur_pressed;

        if (cur_pressed && !old_pressed)
        {
            void (*callback)() = option->callback;
            if (callback)
            {
                callback();
                if (buttons & SCE_CTRL_PSBUTTON)
                    SetPSbuttonEventEnabled(0);
                return 1;
            }
        }
    }

    return 0;
}

static int checkPSbutton(int prot, uint32_t buttons)
{
    uint8_t cur_pressed = ((buttons & SCE_CTRL_PSBUTTON) != 0);
    uint8_t old_pressed = psbutton_old_pressed[prot];
    psbutton_old_pressed[prot] = cur_pressed;

    if (cur_pressed)
    {
        psbutton_hold_count[prot]++;
        if (psbutton_hold_count[prot] >= DISABLE_PSBUTTON_EVENT_HOLD_COUNT)
            SetPSbuttonEventEnabled(0);
    }
    else if (old_pressed)
    {
        if (IsPSbuttonEventEnabled())
        {
            float speed = Emu_GetCurrentRunSpeed();
            if (speed != 1.0f)
            {
                speed = 1.0f;
                Emu_SetRunSpeed(speed);
            }
            else
            {
                Emu_PauseGame();
                GUI_OpenDialog(&setting_dialog);
                SetPSbuttonEventEnabled(0);
                return 1;
            }
        }
    }
    else
    {
        psbutton_hold_count[prot] = 0;
        SetPSbuttonEventEnabled(1);
    }

    return 0;
}

void Emu_PollInput()
{
    SceCtrlData ctrl_data;
    int read_port, local_port, map_port;
    int i;

    memset(retro_keys, 0, sizeof(retro_keys));

    for (local_port = 0; local_port < N_CTRL_PORTS; local_port++)
    {
        if (local_port == 0)
        {
            read_port = 0;
            map_port = control_config.map_port;
        }
        else
        {
            read_port = local_port + 1;
            map_port = local_port;
        }

        memset(&ctrl_data, 0, sizeof(SceCtrlData));
        if (sceCtrlPeekBufferPositiveExt2(read_port, &ctrl_data, 1) < 0)
            continue;

        ctrl_data.buttons &= 0x0001FFFF;

        if (local_port == 0)
            TouchToButtons(&ctrl_data.buttons);
        AnalogToButtons(ctrl_data.lx, ctrl_data.ly, ctrl_data.rx, ctrl_data.ry, &ctrl_data.buttons);

        if (checkPSbutton(local_port, ctrl_data.buttons))
            return;

        if (checkHotKey(local_port, ctrl_data.buttons))
            return;

        if (control_config.left_analog == 1)
            AnalogToDpads(ctrl_data.lx, ctrl_data.ly, &ctrl_data.buttons);

        if (control_config.right_analog == 1)
            AnalogToDpads(ctrl_data.rx, ctrl_data.ry, &ctrl_data.buttons);

        for (i = 0; i < N_KEY_MAP_OPTIONS; i++)
            LocalKeyToEmuKey(&key_map_options[i], local_port, map_port, ctrl_data.buttons);
    }
}

int Emu_InitInput()
{
    cleanInputKeys();
    return 0;
}

int Emu_DeinitInput()
{
    return 0;
}

void Retro_InputPollCallback()
{
}

int16_t Retro_InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    if (port >= N_CTRL_PORTS)
        return 0;

    int16_t res = 0;

    if (device == RETRO_DEVICE_JOYPAD)
    {
        if (retro_input_is_bitmasks)
            res = retro_keys[port] & 0xFFFF;
        else
            res = retro_keys[port] & RETRO_KEY_TO_BITMASK(id);
    }

    return res;
}
