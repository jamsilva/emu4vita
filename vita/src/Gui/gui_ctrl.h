#ifndef __M_GUI_CTRL_H__
#define __M_GUI_CTRL_H__

enum PadButtons
{
    PAD_LEFT,
    PAD_UP,
    PAD_RIGHT,
    PAD_DOWN,
    PAD_CROSS,
    PAD_CIRCLE,
    PAD_SQUARE,
    PAD_TRIANGLE,
    PAD_L1,
    PAD_R1,
    PAD_L2,
    PAD_R2,
    PAD_L3,
    PAD_R3,
    PAD_SELECT,
    PAD_START,
    PAD_PSBUTTON,
    PAD_ENTER,
    PAD_CANCEL,
    PAD_LEFT_ANALOG_UP,
    PAD_LEFT_ANALOG_DOWN,
    PAD_LEFT_ANALOG_LEFT,
    PAD_LEFT_ANALOG_RIGHT,
    PAD_RIGHT_ANALOG_UP,
    PAD_RIGHT_ANALOG_DOWN,
    PAD_RIGHT_ANALOG_LEFT,
    PAD_RIGHT_ANALOG_RIGHT,
    PAD_N_BUTTONS
};
#define N_LOCAL_BUTTONS PAD_PSBUTTON

#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64
#define ANALOG_SENSITIVITY 16

#define N_CTRL_PORTS 4

typedef uint8_t Pad[PAD_N_BUTTONS];

extern Pad old_pad, current_pad, pressed_pad, released_pad, hold_pad, hold2_pad;
extern Pad hold_count, hold2_count, real_hold_count;

void GUI_ReadPad();
void GUI_CleanPad();

#endif