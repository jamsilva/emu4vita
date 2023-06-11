#ifndef __M_ALERT_DIALOG_H__
#define __M_ALERT_DIALOG_H__

#include "List/text_list.h"
#include "Gui/gui.h"

enum TypeAlertDialog
{
    TYPE_DIALOG_TIP,
    TYPE_DIALOG_MENU,
};

typedef struct AlertDialogData
{
    GUI_Dialog *dialog;
    int auto_free;
    int type;
    char *title;
    char *positive_text;
    char *negative_text;
    TextList list;
    int top_pos;
    int focus_pos;
    void (*positiveCallback)(GUI_Dialog *dialog);
    void (*negativeCallback)(GUI_Dialog *dialog);
    void (*freeCallback)(GUI_Dialog *dialog);
    int dialog_width;
    int dialog_height;
    int color_gradual;
    void *userdata;
} AlertDialogData;

GUI_Dialog *AlertDialog_Creat();
void AlertDialog_Destroy(GUI_Dialog *dialog);
void AlertDialog_SetAutoFree(GUI_Dialog *dialog, int auto_free);
void AlertDialog_SetTitle(GUI_Dialog *dialog, char *title);
void AlertDialog_SetMessage(GUI_Dialog *dialog, char *message);
void AlertDialog_SetMenu(GUI_Dialog *dialog, char **list, int list_len);
void AlertDialog_SetPositiveCallback(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog));
void AlertDialog_SetNegativeCallback(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog));
void AlertDialog_SetFreeCallback(GUI_Dialog *dialog, void (*callback)(GUI_Dialog *dialog));

#endif