#ifndef __M_UTILS_H__
#define __M_UTILS_H__

#include <stdint.h>

#include <psp2/rtc.h>
#include <psp2/ctrl.h>
#include <psp2/gxm.h>

#ifdef MIN
#undef MIN
#endif
#ifndef MAX
#undef MAX
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define ANALOG_CENTER 128
#define ANALOG_THRESHOLD 64
#define ANALOG_SENSITIVITY 16

#define SKIP_PRESSED_PSBUTTON_COUNT 30

enum ControlTypes
{
    LIST_CONTROL_NONE,
    LIST_CONTROL_UP,
    LIST_CONTROL_DOWN,
    LIST_CONTROL_SKIP_UP,
    LIST_CONTROL_SKIP_DOWN,
};

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

typedef struct
{
    SceCtrlData pad;
    int error;
} PadList;

typedef uint8_t Pad[PAD_N_BUTTONS];

extern uint32_t vita_buttons[];

extern PadList pad_list[4];
extern Pad old_pads[4], current_pads[4];
extern Pad hold_counts[4];
extern Pad old_pad, current_pad, pressed_pad, released_pad, hold_pad, hold2_pad;
extern Pad hold_count, hold2_count;

int APP_LOG(char *text, ...);

void readPad();
void resetPad();

int hasEndSlash(const char *path);
int removeEndSlash(char *path);
int addEndSlash(char *path);

void convertUtcToLocalTime(SceDateTime *time_local, SceDateTime *time_utc);
void convertLocalTimeToUtc(SceDateTime *time_utc, SceDateTime *time_local);

void getSizeString(char string[16], uint64_t size);
void getDateString(char string[24], int date_format, SceDateTime *time);
void getTimeString(char string[16], int time_format, SceDateTime *time);

void refreshListPos(int *top_pos, int *focus_pos, int length, int lines);
void controlRefreshListPos(int type, int *top_pos, int *focus_pos, int length, int lines);

void initPowerTickThread();

void lockHome();
void unlockHome();
void unlockHomeEx();
void lockUsbConnection();
void unlockUsbConnection();
void lockQuickMenu();
void unlockQuickMenu();

void skipPressedPsbutton();
void unskipPressedPsbutton();
int isSkipPressedPsbutton();

#endif
