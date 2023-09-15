#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "list/file_list.h"
#include "setting/setting.h"
#include "emu/emu.h"
#include "gui/gui.h"
#include "browser.h"
#include "utils.h"
#include "file.h"
#include "config.h"
#include "lang.h"

extern GUI_Activity about_activity;

static int startActivityCallback(GUI_Activity *activity);
static int exitActivityCallback(GUI_Activity *activity);
static void drawActivityCallback(GUI_Activity *activity);
static void ctrlActivityCallback(GUI_Activity *activity);

static GUI_ButtonInstruction button_instructions[] = {
    {BUTTON_CANCEL, PARENT_DIRECTORY, 1},
    {BUTTON_ENTER, OPEN_DIR, 1},
    {BUTTON_TRIANGLE, OPTION_MENU, 1},
    {BUTTON_PSBUTTON, SETTING_MENU, 1},
    {BUTTON_SELECT, ABOUT, 1},
    {BUTTON_START, CHANGE_DIR, 1},
    {LANG_NULL, LANG_NULL, 0},
};

GUI_Activity browser_activity = {
    APP_TITLE,             // Title
    button_instructions,   // Button instructions
    NULL,                  // Wallpaper
    startActivityCallback, // Start callback
    exitActivityCallback,  // Exit callback
    drawActivityCallback,  // Draw callback
    ctrlActivityCallback,  // Ctrl callback
    0,                     // Disable draw statusbar
    NULL,                  // Parent activity
    NULL,                  // User data
};

enum IndexOptionItem
{
    INDEX_OPTION_ITEM_LOAD_GAME,
    INDEX_OPTION_ITEM_DELETE_GAME,
    INDEX_OPTION_ITEM_DELETE_AUTO_STATE,
    INDEX_OPTION_ITEM_DELETE_SAVEFILE,
};

static int option_items[] = {
    LABEL_START_GAME,
    LABEL_DELETE_GAME,
    LABEL_DELETE_AUTO_STATE,
    LABEL_DELETE_SAVEFILE,
};
#define N_OPTION_ITEMS (sizeof(option_items) / sizeof(int))

#define MAX_DIR_LEVELS 128

#define LAYOUT_CHILD_MARGIN 2

#define PATH_VIEW_PADDING_L 10
#define PATH_VIEW_PADDING_T 6
#define PATH_TEXT_COLOR COLOR_SPRING_GREEN

#define LISTVIEW_PADDING_L 2
#define LISTVIEW_PADDING_T 2
#define ITEMVIEW_PADDING_L 8
#define ITEMVIEW_PADDING_T 5

#define ITEMVIEW_COLOR_BG 0
#define ITEMVIEW_COLOR_FOCUS GUI_DEF_COLOR_FOCUS
#define FOLDER_TEXT_COLOR COLOR_WHITE
#define FILE_TEXT_COLOR COLOR_GREEN

#define PREVIEW_VIEW_PADDING 8
#define PREVIEW_REFRESH_DELAY_FRAMES 6

// File list
static LinkedList *file_list = NULL;
static int list_sort_mode = SORT_BY_NAME;

// Position
static int focus_pos_saves[MAX_DIR_LEVELS] = {0};
static int dir_level = 0;

// Preview
static int preview_need_refresh = 1;
static int preview_refresh_delay_frames = 0;

static Layout *browser_layout = NULL;
static TextView *path_textview = NULL;
static ListView *broeser_listview = NULL;
static ImageView *preview_imageview = NULL;

static int getListLength(void *list)
{
    return LinkedListGetLength((LinkedList *)list);
}

static void *getHeadListEntry(void *list)
{
    return LinkedListHead((LinkedList *)list);
}

static void *getTailListEntry(void *list)
{
    return LinkedListTail((LinkedList *)list);
}

static void *getNextListEntry(void *list, void *cur_entry, int cur_idx)
{
    return LinkedListNext((LinkedListEntry *)cur_entry);
}

static void *getPrevListEntry(void *list, void *cur_entry, int cur_idx)
{
    return LinkedListPrev((LinkedListEntry *)cur_entry);
}

static char *getName(void *list, void *entry, int idx)
{
    if (!entry)
        return NULL;

    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData((LinkedListEntry *)entry);
    return data->name;
}

static uint32_t getNameColor(void *list, void *entry, int idx)
{
    if (!entry)
        return FOLDER_TEXT_COLOR;

    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData((LinkedListEntry *)entry);
    return data->is_folder ? FOLDER_TEXT_COLOR : FILE_TEXT_COLOR;
}

static int refreshLayout()
{
    int layout_w = 0, layout_h = 0;
    int remaining_w = 0, remaining_h = 0;

    GUI_GetActivityLayoutWH(&browser_activity, &layout_w, &layout_h);

    if (!browser_layout)
    {
        browser_layout = NewLayout(TYPE_LAYOUT_ORIENTATION_VERTICAL, layout_w, layout_h);
        if (!browser_layout)
            return -1;
        LayoutParamSetAutoFree(browser_layout, 0);
    }
    LayoutSetOrientation(browser_layout, TYPE_LAYOUT_ORIENTATION_VERTICAL);
    LayoutParamSetLayoutSize(browser_layout, layout_w, layout_h);
    LayoutParamSetPadding(browser_layout, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING, GUI_DEF_MAIN_LAYOUT_PADDING);
    LayoutEmpty(browser_layout);

    if (!path_textview)
    {
        path_textview = NewTextView();
        if (!path_textview)
            return -1;
        LayoutParamSetAutoFree(path_textview, 0);
    }
    LayoutParamSetLayoutSize(path_textview, TYPE_LAYOUT_MATH_PARENT, GUI_GetFontSize() + PATH_VIEW_PADDING_T * 2);
    LayoutParamSetPadding(path_textview, PATH_VIEW_PADDING_L, PATH_VIEW_PADDING_L, PATH_VIEW_PADDING_T, PATH_VIEW_PADDING_T);
    TextViewSetSingleLine(path_textview, 1);
    TextViewSetBgColor(path_textview, GUI_DEF_COLOR_BG);
    TextViewSetTextColor(path_textview, PATH_TEXT_COLOR);
    LayoutAdd(browser_layout, path_textview);

    Layout *ls_layout = NewLayout();
    LayoutSetOrientation(ls_layout, TYPE_LAYOUT_ORIENTATION_HORIZONTAL);
    LayoutParamSetLayoutSize(ls_layout, TYPE_LAYOUT_MATH_PARENT, TYPE_LAYOUT_MATH_PARENT);
    LayoutParamSetMargin(ls_layout, 0, 0, LAYOUT_CHILD_MARGIN, 0);
    LayoutAdd(browser_layout, ls_layout);

    ViewRefresh(ls_layout);
    LayoutGetRemaining(ls_layout, &remaining_w, &remaining_h);

    if (!preview_imageview)
    {
        preview_imageview = NewImageView();
        if (!preview_imageview)
            return -1;
        LayoutParamSetAutoFree(preview_imageview, 0);
    }
    LayoutParamSetLayoutSize(preview_imageview, remaining_h, remaining_h);
    LayoutParamSetPadding(preview_imageview, PREVIEW_VIEW_PADDING, PREVIEW_VIEW_PADDING, PREVIEW_VIEW_PADDING, PREVIEW_VIEW_PADDING);
    ImageViewSetBgColor(preview_imageview, GUI_DEF_COLOR_BG);
    LayoutAdd(ls_layout, preview_imageview);

    if (!broeser_listview)
    {
        broeser_listview = NewListView();
        if (!broeser_listview)
            return -1;
        LayoutParamSetAutoFree(broeser_listview, 0);
    }
    LayoutParamSetLayoutSize(broeser_listview, TYPE_LAYOUT_MATH_PARENT, TYPE_LAYOUT_MATH_PARENT);
    LayoutParamSetMargin(broeser_listview, 0, LAYOUT_CHILD_MARGIN, 0, 0);
    LayoutParamSetPadding(broeser_listview, LISTVIEW_PADDING_L, LISTVIEW_PADDING_L, LISTVIEW_PADDING_T, LISTVIEW_PADDING_T);
    ListViewSetBgColor(broeser_listview, GUI_DEF_COLOR_BG);
    ListViewSetItemPadding(broeser_listview, ITEMVIEW_PADDING_L, ITEMVIEW_PADDING_L, ITEMVIEW_PADDING_T, ITEMVIEW_PADDING_T);
    ListViewSetItemFocusColor(broeser_listview, ITEMVIEW_COLOR_FOCUS);
    ListViewSetFocusPosEnabled(broeser_listview, 1);
    ViewAddAbove(preview_imageview, broeser_listview);

    ViewRefresh(browser_layout);

    return 0;
}

int CurrentPathIsFile()
{
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (data)
        return !data->is_folder;

    return 0;
}

int MakeCurrentFileName(char *name)
{
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!data || !data->name)
        return -1;

    snprintf(name, MAX_NAME_LENGTH, data->name);
    return 0;
}

int MakeCurrentFilePath(char *path)
{
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!ls_data || !e_data || !e_data->name)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, "%s%s", ls_data->path, e_data->name);
    return 0;
}

int MakePreviewPath(char *path)
{
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!ls_data || !e_data || !e_data->name)
        return -1;

    char base_name[MAX_NAME_LENGTH];
    int ret = MakeBaseName(base_name, e_data->name, MAX_NAME_LENGTH);
    if (ret < 0)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, ("%s/%s/%s.png"), ls_data->path, PREVIEW_DIR_NAME, base_name);
    return 0;
}

int MakeScreenshotPath(char *path)
{
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!data || !data->name)
        return -1;

    char base_name[MAX_NAME_LENGTH];
    int ret = MakeBaseName(base_name, data->name, MAX_NAME_LENGTH);
    if (ret < 0)
        return -1;

    int i;
    for (i = 0; i < 1000; i++)
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s_%d.png", CORE_SCREENSHOTS_DIR, base_name, i);
        if (!CheckFileExist(path))
            return 1;
    }

    return 0;
}

GUI_Texture *GetDefaultPreviewTexture()
{
    char path[MAX_PATH_LENGTH];
    MakePreviewPath(path);
    GUI_Texture *texture = GUI_LoadPNGFile(path);

    return texture;
}

void Browser_RequestRefreshPreview(int urgent)
{
    preview_need_refresh = 1;

    if (urgent)
        preview_refresh_delay_frames = 0;
    else
        preview_refresh_delay_frames = PREVIEW_REFRESH_DELAY_FRAMES;

    ImageViewSetTexture(preview_imageview, NULL, 1);
}

static void moveFileListPos(int type)
{
    static int old_dir_level = -1;
    static int old_focus_pos = -1;

    ListViewMovePos(broeser_listview, type);
    int focus_pos = ListViewGetFocusPos(broeser_listview);

    if (old_dir_level != dir_level || old_focus_pos != focus_pos)
    {
        Browser_RequestRefreshPreview(0);
        Setting_SetStateSelectId(0);
    }

    if (CurrentPathIsFile())
    {
        button_instructions[1].instruction = START_GAME;
        button_instructions[2].visibility = 1;
    }
    else
    {
        button_instructions[1].instruction = OPEN_DIR;
        button_instructions[2].visibility = 0;
    }

    old_dir_level = dir_level;
    old_focus_pos = focus_pos;
}

static void dirLevelUp()
{
    if (dir_level < MAX_DIR_LEVELS - 1)
    {
        focus_pos_saves[dir_level] = ListViewGetFocusPos(broeser_listview);
        dir_level++;
    }

    ListViewSetFocusPos(broeser_listview, 0);
}

static void dirLevelDown()
{
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    RemoveEndSlash(ls_data->path);

    char *p;
    p = strrchr(ls_data->path, '/'); // ux0:abc/cde
    if (p)
    {
        p[1] = '\0';
        dir_level--;
        goto DIR_UP_RETURN;
    }

    p = strrchr(ls_data->path, ':'); // ux0:abc || ux0:
    if (p)
    {
        if (strlen(ls_data->path) - ((p + 1) - ls_data->path) > 0) // ux0:abc
        {
            p[1] = '\0';
            dir_level--;
            goto DIR_UP_RETURN;
        }
    }

    strcpy(ls_data->path, HOME_PATH);
    dir_level = 0;

DIR_UP_RETURN:
    if (dir_level < 0)
        dir_level = 0;

    ListViewSetFocusPos(broeser_listview, focus_pos_saves[dir_level]);
}

static int setFocusByName(const char *name)
{
    int l_length = LinkedListGetLength(file_list);
    int pos = FileListGetNumberByName(file_list, name);
    if (pos < 0 || pos >= l_length)
        return -1;

    ListViewSetFocusPos(broeser_listview, pos);
    moveFileListPos(TYPE_MOVE_NONE);
    return 0;
}

static int refreshFileList()
{
    int ret = 0, res = 0;
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);

    do
    {
        LinkedListEmpty(file_list);
        res = FileListGetEntries(file_list, ls_data->path, list_sort_mode);
        if (res < 0)
        {
            ret = res;
            dirLevelDown();
        }
    } while (res < 0);

    TextViewSetText(path_textview, ls_data->path);
    ListViewRefreshtList(broeser_listview);
    moveFileListPos(TYPE_MOVE_NONE);

    return ret;
}

static int changeToDir(char *lastdir)
{
    if (!CheckFolderExist(lastdir))
        return -1;

    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    dir_level = 0;
    strcpy(ls_data->path, HOME_PATH);

    int i;
    for (i = 0; i < strlen(lastdir) + 1; i++)
    {
        if (lastdir[i] == ':' || lastdir[i] == '/')
        {
            char ch = lastdir[i + 1];
            lastdir[i + 1] = '\0';

            char ch2 = lastdir[i];
            lastdir[i] = '\0';

            char *p = strrchr(lastdir, '/');
            if (!p)
                p = strrchr(lastdir, ':');
            if (!p)
                p = lastdir - 1;

            lastdir[i] = ch2;

            refreshFileList();
            setFocusByName(p + 1);

            strcpy(ls_data->path, lastdir);

            lastdir[i + 1] = ch;

            dirLevelUp();
        }
    }
    refreshFileList();

    return 0;
}

int Browser_ChangeDirByFilePath(const char *path)
{
    int ret;

    char lastdir[MAX_PATH_LENGTH];
    ret = MakeBaseDirectory(lastdir, path, MAX_PATH_LENGTH);
    if (ret < 0)
        return ret;

    ret = changeToDir(lastdir);
    if (ret < 0)
        return ret;

    char name[MAX_NAME_LENGTH];
    ret = MakeFilename(name, path, MAX_NAME_LENGTH);
    if (ret >= 0)
        setFocusByName(name);

    return 0;
}

int Browser_ChangeDirBySaveFile(const char *path)
{
    char lastfile[MAX_PATH_LENGTH];
    if (ReadFile(path, lastfile, sizeof(lastfile)) <= 0)
        return -1;
    lastfile[MAX_PATH_LENGTH - 1] = '\0';

    return Browser_ChangeDirByFilePath(lastfile);
}

static void backToParentDir()
{
    if (dir_level > 0)
    {
        dirLevelDown();
        refreshFileList();
    }
}

static void enterToChildDir(LinkedListEntry *entry)
{
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!ls_data || !e_data)
        return;

    if (dir_level == 0)
    {
        strcpy(ls_data->path, e_data->name);
    }
    else
    {
        if (dir_level > 1)
            AddEndSlash(ls_data->path);
        strcat(ls_data->path, e_data->name);
    }

    dirLevelUp();
    refreshFileList();
}

static void startGame(LinkedListEntry *entry)
{
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!ls_data || !e_data)
        return;

    EmuGameInfo info;
    snprintf(info.path, MAX_PATH_LENGTH, "%s%s", ls_data->path, e_data->name);
    info.state_num = -2;
    Emu_StartGame(&info);
}

static void refreshPreviewImageView()
{
    switch (app_config.preview_style)
    {
    case TYPE_PREVIEW_SCALE_TYPE_FIT_XY:
        ImageViewSetScaleType(preview_imageview, TYPE_IMAGE_SCALE_FIT_XY);
        break;
    case TYPE_PREVIEW_SCALE_TYPE_FIT_CENTER_CROP:
        ImageViewSetScaleType(preview_imageview, TYPE_IMAGE_SCALE_FIT_CENTER_CROP);
        break;
    case TYPE_PREVIEW_SCALE_TYPE_FIT_CENTER_INSIDE:
    default:
        ImageViewSetScaleType(preview_imageview, TYPE_IMAGE_SCALE_FIT_CENTER_INSIDE);
        break;
    }
    ViewRefresh(preview_imageview);
}

static void refreshPreview()
{
    GUI_Texture *tex = NULL;

    if (CurrentPathIsFile())
    {
        switch (app_config.preview_path)
        {
        case TYPE_PREVIEW_PATH_DEFAULT:
            tex = GetDefaultPreviewTexture();
            break;
        case TYPE_PREVIEW_PATH_SAVESTATE:
            tex = Emu_GetStateScreenshotTexture(-1);
            break;

        case TYPE_PREVIEW_PATH_AUTO:
        default:
            if (misc_config.auto_save_load)
                tex = Emu_GetStateScreenshotTexture(-1);
            if (!tex)
                tex = GetDefaultPreviewTexture();
            break;
        }
    }

    ImageViewSetTexture(preview_imageview, tex, 1);
    refreshPreviewImageView();
}

static void onPreviewUpdateEvent()
{
    if (!preview_need_refresh)
        return;

    if (preview_refresh_delay_frames > 0)
    {
        preview_refresh_delay_frames--;
    }
    else
    {
        refreshPreview();
        preview_need_refresh = 0;
    }
}

static void drawActivityCallback(GUI_Activity *activity)
{
    int layout_x = 0, layout_y = 0;
    GUI_GetActivityLayoutXY(&browser_activity, &layout_x, &layout_y);

    onPreviewUpdateEvent();
    LayoutDraw(browser_layout, layout_x, layout_y);
}

static void deleteGameCallback(GUI_Dialog *dialog)
{
    char path[MAX_PATH_LENGTH];
    if (MakeCurrentFilePath(path) < 0)
        return;

    sceIoRemove(path);
#ifdef FBA_BUILD
    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    char base_name[MAX_NAME_LENGTH];
    MakeBaseName(base_name, path, MAX_NAME_LENGTH);
    snprintf(path, MAX_PATH_LENGTH, "%s%s.dat", ls_data->path, base_name);
    sceIoRemove(path);
#endif
    GUI_CloseDialog(dialog->prev);
    AlertDialog_Dismiss(dialog);
    refreshFileList(file_list);
    ListViewRefreshtList(broeser_listview);
    Browser_RequestRefreshPreview(0);
}

static void deleteAutoStateCallback(GUI_Dialog *dialog)
{
    Emu_DeleteState(-1);
    GUI_CloseDialog(dialog->prev);
    AlertDialog_Dismiss(dialog);
    Browser_RequestRefreshPreview(0);
}

static void deleteSrmCallback(GUI_Dialog *dialog)
{
    Emu_DeleteSrm();
    GUI_CloseDialog(dialog->prev);
    AlertDialog_Dismiss(dialog);
}

static void optionMenuPositiveCallback(GUI_Dialog *dialog)
{
    if (!dialog || !dialog->userdata)
        return;

    int focus_pos = ListViewGetFocusPos(broeser_listview);
    AlertDialogData *data = (AlertDialogData *)dialog->userdata;

    switch (data->focus_pos)
    {
    case INDEX_OPTION_ITEM_LOAD_GAME:
    {
        LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
        FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
        if (data && !data->is_folder)
        {
            GUI_CloseAllDialogs(TYPE_GUI_DIALOG_ANY);
            startGame(entry);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_GAME:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Create();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TITLE_TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[MESSAGE_ASK_DELETE_GAME]);
            AlertDialog_SetPositiveButton(tip_dialog, cur_lang[CONFIRM], deleteGameCallback);
            AlertDialog_SetNegativeButton(tip_dialog, cur_lang[CANCEL], NULL);
            AlertDialog_Show(tip_dialog);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_AUTO_STATE:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Create();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TITLE_TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[MESSAGE_ASK_DELETE_AUTO_STATE]);
            AlertDialog_SetPositiveButton(tip_dialog, cur_lang[CONFIRM], deleteAutoStateCallback);
            AlertDialog_SetNegativeButton(tip_dialog, cur_lang[CANCEL], NULL);
            AlertDialog_Show(tip_dialog);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_SAVEFILE:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Create();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TITLE_TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[MESSAGE_ASK_DELETE_SAVEFILE]);
            AlertDialog_SetPositiveButton(tip_dialog, cur_lang[CONFIRM], deleteSrmCallback);
            AlertDialog_SetNegativeButton(tip_dialog, cur_lang[CANCEL], NULL);
            AlertDialog_Show(tip_dialog);
        }
    }
    break;
    default:
        break;
    }
}

static void openOptionMenu()
{
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!data || data->is_folder)
        return;

    GUI_Dialog *dialog = AlertDialog_Create();
    AlertDialog_SetTitle(dialog, cur_lang[TITLE_MENU]);
    int n_items = N_OPTION_ITEMS;
    char **items = GUI_GetStringArrayByIdArray(option_items, n_items);
    AlertDialog_SetItems(dialog, items, n_items);
    free(items);
    AlertDialog_SetPositiveButton(dialog, cur_lang[CONFIRM], optionMenuPositiveCallback);
    AlertDialog_SetNegativeButton(dialog, cur_lang[CANCEL], NULL);
    AlertDialog_Show(dialog);
}

static void onItemClick()
{
    int focus_pos = ListViewGetFocusPos(broeser_listview);
    LinkedListEntry *entry = LinkedListFindByNum(file_list, focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (data)
    {
        if (data->is_folder)
            enterToChildDir(entry);
        else
            startGame(entry);
    }
}

static void ctrlActivityCallback(GUI_Activity *activity)
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        moveFileListPos(TYPE_MOVE_UP);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        moveFileListPos(TYPE_MOVE_DOWN);
    }
    else if (hold_pad[PAD_LEFT])
    {
        moveFileListPos(TYPE_MOVE_LEFT);
    }
    else if (hold_pad[PAD_RIGHT])
    {
        moveFileListPos(TYPE_MOVE_RIGHT);
    }

    if (released_pad[PAD_TRIANGLE])
    {
        openOptionMenu();
    }
    else if (released_pad[PAD_CANCEL])
    {
        backToParentDir();
    }
    else if (released_pad[PAD_ENTER])
    {
        onItemClick();
    }
    else if (released_pad[PAD_SELECT])
    {
        GUI_StartActivity(&about_activity);
    }
    else if (released_pad[PAD_START])
    {
        Browser_ChangeDirBySaveFile(LASTFILE_PATH);
    }
}

static int startActivityCallback(GUI_Activity *activity)
{
    browser_activity.wallpaper = GUI_GetDefaultWallpaper();
    refreshLayout();

    if (!file_list)
        file_list = NewFileList();
    if (!file_list)
        return -1;

    ListViewCallbacks callbacks;
    memset(&callbacks, 0, sizeof(ListViewCallbacks));
    callbacks.getListLength = getListLength;
    callbacks.getHeadListEntry = getHeadListEntry;
    callbacks.getTailListEntry = getTailListEntry;
    callbacks.getNextListEntry = getNextListEntry;
    callbacks.getPrevListEntry = getPrevListEntry;
    callbacks.getName = getName;
    callbacks.getNameColor = getNameColor;
    ListViewSetList(broeser_listview, file_list, &callbacks);

    FileListData *ls_data = (FileListData *)LinkedListGetListData(file_list);
    strcpy(ls_data->path, HOME_PATH);
    refreshFileList();
    Browser_ChangeDirBySaveFile(LASTFILE_PATH);

    return 0;
}

static int exitActivityCallback(GUI_Activity *activity)
{
    LinkedListEmpty(file_list);
    ListViewEmptyItems(broeser_listview);
    return 0;
}
