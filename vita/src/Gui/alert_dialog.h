#ifndef __M_ALERT_DIALOG_H__
#define __M_ALERT_DIALOG_H__

#include "list/string_list.h"
#include "gui/gui.h"

enum TypeAlertDialog
{
    TYPE_ALERT_DIALOG_TIP,
    TYPE_ALERT_DIALOG_MENU,
};

enum TypeAlertDialogStatus
{
    TYPE_DIALOG_STATUS_SHOW,
    TYPE_DIALOG_STATUS_DISMISS,
};

typedef struct AlertDialogData
{
    GUI_Dialog *dialog;
    int opened;
    int auto_free;
    int type;
    char *title;
    StringList list;
    int top_pos;
    int focus_pos;
    char *positive_text;
    char *negative_text;
    char *neutral_text;
    void (*positiveCallback)(GUI_Dialog *dialog);
    void (*negativeCallback)(GUI_Dialog *dialog);
    void (*neutralCallback)(GUI_Dialog *dialog);
    void (*freeCallback)(GUI_Dialog *dialog);
    int status;
    int dialog_width;
    int dialog_height;
    int gradual_count;
    int listview_n_draw_items;
    void *userdata;
} AlertDialogData;

GUI_Dialog *AlertDialog_Creat();
void AlertDialog_Destroy(GUI_Dialog *dialog);
void AlertDialog_SetAutoFree(GUI_Dialog *dialog, int auto_free);
void AlertDialog_SetUserdata(GUI_Dialog *dialog, void *userdata);
void AlertDialog_SetTitle(GUI_Dialog *dialog, char *title);
void AlertDialog_SetMessage(GUI_Dialog *dialog, char *message);
void AlertDialog_SetItems(GUI_Dialog *dialog, char **items, int n_items);
void AlertDialog_SetPositiveButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog));
void AlertDialog_SetNegativeButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog));
void AlertDialog_setNeutralButton(GUI_Dialog *dialog, char *name, void (*callback)(GUI_Dialog *dialog));
void AlertDialog_SetFreeCallback(GUI_Dialog *dialog, void (*callback)(GUI_Dialog *dialog));

int AlertDialog_ShowSimpleTipDialog(char *title, char *message);
int AlertDialog_Show(GUI_Dialog *dialog);
int AlertDialog_Dismiss(GUI_Dialog *dialog);

#endif