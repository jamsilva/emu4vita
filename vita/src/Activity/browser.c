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
#define PATH_VIEW_HEIGHT (GUI_GetFontSize() + PATH_VIEW_PADDING_T * 2)
#define PATH_TEXT_COLOR COLOR_SPRING_GREEN

#define NAME_LISTVIEW_PADDING_L 2
#define NAME_LISTVIEW_PADDING_T 2
#define NAME_ITEMVIEW_PADDING_L 8
#define NAME_ITEMVIEW_PADDING_T 5
#define NAME_ITEMVIEW_HEIGHT (GUI_GetFontSize() + NAME_ITEMVIEW_PADDING_T * 2)
#define NAME_TEXT_COLOR_FOLDER COLOR_WHITE
#define NAME_TEXT_COLOR_FILE COLOR_GREEN
#define NAME_ITEMVIEW_COLOR_FOCUS_BG GUI_DEF_COLOR_FOCUS_BG

#define PREVIEW_VIEW_PADDING 8
#define PREVIEW_REFRESH_DELAY_FRAMES 6

// File list
static LinkedList *file_list = NULL;
static int list_sort_mode = SORT_BY_NAME;

// Position
static int listview_top_pos = 0, listview_focus_pos = 0;
static int top_pos_saves[MAX_DIR_LEVELS] = {0}, focus_pos_saves[MAX_DIR_LEVELS] = {0};
static int dir_level = 0;

// Preview
static GUI_Texture *preview_tex = NULL;
static int preview_need_refresh = 1;
static int preview_refresh_delay_frames = 0;

// Layout
int layout_x, layout_y;
int layout_w, layout_h;

int path_textview_w, path_textview_h;
int path_textview_x, path_textview_y, path_textview_dx, path_textview_dy;

int name_listview_x, name_listview_y;
int name_listview_w, name_listview_h;
int name_itemview_w, name_itemview_h;
int name_listview_n_draw_items;
int name_scroll_count = 0;

int name_scrollbar_track_x, name_scrollbar_track_y;
int name_scrollbar_track_height;

int preview_imageview_x, preview_imageview_y;
int preview_imageview_w, preview_imageview_h;
int preview_tex_x, preview_tex_y;
float preview_tex_x_scale, preview_tex_y_scale;

static void refreshLayout()
{
    int layout_x, layout_y;
    int layout_w, layout_h;
    int layout_dx, layout_dy;

    GUI_GetActivityLayoutXY(&browser_activity, &layout_x, &layout_y);
    GUI_GetActivityLayoutWH(&browser_activity, &layout_w, &layout_h);

    layout_x = layout_x + GUI_DEF_MAIN_LAYOUT_PADDING;
    layout_y = layout_y + GUI_DEF_MAIN_LAYOUT_PADDING;
    layout_w = layout_w - (GUI_DEF_MAIN_LAYOUT_PADDING * 2);
    layout_h = layout_h - (GUI_DEF_MAIN_LAYOUT_PADDING * 2);
    layout_dx = layout_x + layout_w;
    layout_dy = layout_y + layout_h;

    path_textview_x = layout_x;
    path_textview_y = layout_y;
    path_textview_w = layout_w;
    path_textview_h = PATH_VIEW_HEIGHT;

    preview_imageview_y = path_textview_y + path_textview_h + LAYOUT_CHILD_MARGIN;
    preview_imageview_h = layout_dy - preview_imageview_y;
    preview_imageview_w = preview_imageview_h;
    preview_imageview_x = layout_dx - preview_imageview_w;

    name_listview_x = layout_x;
    name_listview_y = preview_imageview_y;
    name_listview_w = preview_imageview_x - LAYOUT_CHILD_MARGIN - name_listview_x;
    name_listview_h = preview_imageview_h;

    name_itemview_w = name_listview_w - NAME_LISTVIEW_PADDING_L * 2;
    name_itemview_h = NAME_ITEMVIEW_HEIGHT;
    name_listview_n_draw_items = (name_listview_h - NAME_LISTVIEW_PADDING_T * 2) / name_itemview_h;

    name_scrollbar_track_x = name_listview_x + name_listview_w - GUI_DEF_SCROLLBAR_SIZE - 2;
    name_scrollbar_track_y = name_listview_y + 2;
    name_scrollbar_track_height = name_listview_h - 4;
}

int CurrentPathIsFile()
{
    LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (data)
        return !data->is_folder;

    return 0;
}

int MakeCurrentFileName(char *name)
{
    LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!data || !data->name)
        return -1;

    snprintf(name, MAX_NAME_LENGTH, data->name);
    return 0;
}

int MakeCurrentFilePath(char *path)
{
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    LinkedListEntry *f_entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(f_entry);
    if (!l_data || !e_data || !e_data->name)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, "%s%s", l_data->path, e_data->name);
    return 0;
}

int MakePreviewPath(char *path)
{
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    LinkedListEntry *f_entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(f_entry);
    if (!l_data || !e_data || !e_data->name)
        return -1;

    char base_name[MAX_NAME_LENGTH];
    int ret = MakeBaseName(base_name, e_data->name, MAX_NAME_LENGTH);
    if (ret < 0)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, ("%s/%s/%s.png"), l_data->path, PREVIEW_DIR_NAME, base_name);
    return 0;
}

int MakeScreenshotPath(char *path)
{
    LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
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

    if (preview_tex)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(preview_tex);
        preview_tex = NULL;
    }
}

static void moveFileListPos(int type)
{
    static int old_dir_level = -1;
    static int old_focus_pos = -1;

    int l_length = LinkedListGetLength(file_list);
    MoveListPos(type, &listview_top_pos, &listview_focus_pos, l_length, name_listview_n_draw_items);
    if (old_dir_level != dir_level || old_focus_pos != listview_focus_pos)
    {
        name_scroll_count = 0;
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
    old_focus_pos = listview_focus_pos;
}

static void dirLevelUp()
{
    if (dir_level < MAX_DIR_LEVELS - 1)
    {
        top_pos_saves[dir_level] = listview_top_pos;
        focus_pos_saves[dir_level] = listview_focus_pos;
        dir_level++;
    }

    listview_top_pos = 0;
    listview_focus_pos = 0;
}

static void dirLevelDown()
{
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    RemoveEndSlash(l_data->path);

    char *p;
    p = strrchr(l_data->path, '/'); // ux0:abc/cde
    if (p)
    {
        p[1] = '\0';
        dir_level--;
        goto DIR_UP_RETURN;
    }

    p = strrchr(l_data->path, ':'); // ux0:abc || ux0:
    if (p)
    {
        if (strlen(l_data->path) - ((p + 1) - l_data->path) > 0) // ux0:abc
        {
            p[1] = '\0';
            dir_level--;
            goto DIR_UP_RETURN;
        }
    }

    strcpy(l_data->path, HOME_PATH);
    dir_level = 0;

DIR_UP_RETURN:
    if (dir_level < 0)
        dir_level = 0;

    listview_top_pos = top_pos_saves[dir_level];
    listview_focus_pos = focus_pos_saves[dir_level];
}

static int setFocusByName(const char *name)
{
    int l_length = LinkedListGetLength(file_list);
    int pos = FileListGetNumberByName(file_list, name);
    if (pos < 0 || pos >= l_length)
        return -1;

    listview_focus_pos = pos;
    moveFileListPos(TYPE_MOVE_NONE);
    return 0;
}

static int refreshFileList()
{
    int ret = 0, res = 0;
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);

    do
    {
        LinkedListEmpty(file_list);
        res = FileListGetEntries(file_list, l_data->path, list_sort_mode);
        if (res < 0)
        {
            ret = res;
            dirLevelDown();
        }
    } while (res < 0);

    moveFileListPos(TYPE_MOVE_NONE);
    return ret;
}

static int changeToDir(char *lastdir)
{
    if (!CheckFolderExist(lastdir))
        return -1;

    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    dir_level = 0;
    strcpy(l_data->path, HOME_PATH);

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

            strcpy(l_data->path, lastdir);

            lastdir[i + 1] = ch;

            dirLevelUp();
        }
    }
    refreshFileList();

    return 0;
}

int Browser_ChangeDirByPath(const char *path)
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

int Browser_ChangeDirByFile(const char *path)
{
    char lastfile[MAX_PATH_LENGTH];
    if (ReadFile(path, lastfile, sizeof(lastfile)) <= 0)
        return -1;
    lastfile[MAX_PATH_LENGTH - 1] = '\0';

    return Browser_ChangeDirByPath(lastfile);
}

static void backToParentDir()
{
    if (dir_level > 0)
    {
        dirLevelDown();
        refreshFileList();
    }
}

static void enterChildDir(LinkedListEntry *entry)
{
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!l_data || !e_data)
        return;

    if (dir_level == 0)
    {
        strcpy(l_data->path, e_data->name);
    }
    else
    {
        if (dir_level > 1)
            AddEndSlash(l_data->path);
        strcat(l_data->path, e_data->name);
    }

    dirLevelUp();
    refreshFileList();
}

static void startGame(LinkedListEntry *entry)
{
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!l_data || !e_data)
        return;

    EmuGameInfo info;
    snprintf(info.path, MAX_PATH_LENGTH, "%s%s", l_data->path, e_data->name);
    info.state_num = -2;
    Emu_StartGame(&info);
}

static void makePreviewDisplayWH(int *w, int *h, int max_w, int max_h)
{
    float ratio;

    switch (app_config.preview_style)
    {
    case TYPE_PREVIEW_STYLE_FULL_STRETCH:
        *w = max_w;
        *h = max_h;
        return;
    case TYPE_PREVIEW_STYLE_FULL_4_3:
        ratio = 4.f / 3.f;
        break;
    case TYPE_PREVIEW_STYLE_FULL_3_2:
        ratio = 3.f / 2.f;
        break;
    default:
        ratio = (float)*w / (float)*h;
        break;
    }

    // new_w / new_h = w / h
    // ==> new_w = new_h * (w / h)
    // ==> new_h = new_w / (w / h)

    // Full max
    int new_w = max_w;
    int new_h = (float)new_w / ratio;
    // Fix above
    if (new_h > max_h)
    {
        new_h = max_h;
        new_w = (float)new_h * ratio;
    }

    *w = new_w;
    *h = new_h;
}

static void refreshPreviewView()
{
    preview_tex_x = preview_imageview_x + PREVIEW_VIEW_PADDING;
    preview_tex_y = preview_imageview_y + PREVIEW_VIEW_PADDING;
    int max_w = preview_imageview_w - PREVIEW_VIEW_PADDING * 2;
    int max_h = preview_imageview_h - PREVIEW_VIEW_PADDING * 2;
    int tex_w = GUI_GetTextureWidth(preview_tex);
    int tex_h = GUI_GetTextureHeight(preview_tex);
    int new_w = tex_w;
    int new_h = tex_h;
    makePreviewDisplayWH(&new_w, &new_h, max_w, max_h);
    preview_tex_x += ((max_w - new_w) / 2);
    preview_tex_y += ((max_h - new_h) / 2);
    preview_tex_x_scale = (float)new_w / (float)tex_w;
    preview_tex_y_scale = (float)new_h / (float)tex_h;
}

static void refreshPreview()
{
    if (preview_tex)
    {
        GUI_WaitRenderingDone();
        GUI_DestroyTexture(preview_tex);
        preview_tex = NULL;
    }

    if (!CurrentPathIsFile())
        return;

    if (app_config.preview_path == TYPE_PREVIEW_PATH_AUTO)
    {
        if (misc_config.auto_save_load)
            preview_tex = Emu_GetStateScreenshotTexture(-1);
        if (!preview_tex)
            preview_tex = GetDefaultPreviewTexture();
    }
    else if (app_config.preview_path == TYPE_PREVIEW_PATH_DEFAULT)
    {
        preview_tex = GetDefaultPreviewTexture();
    }
    else if (app_config.preview_path == TYPE_PREVIEW_PATH_SAVESTATE)
    {
        preview_tex = Emu_GetStateScreenshotTexture(-1);
    }

    if (preview_tex)
        refreshPreviewView();
}

static void checkPreview()
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
    int x, y;
    int w, clip_w, clip_h;
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    int l_length = LinkedListGetLength(file_list);

    checkPreview();

    // Draw path bg
    GUI_DrawFillRectangle(path_textview_x, path_textview_y, path_textview_w, path_textview_h, GUI_DEF_COLOR_BG);
    // Draw name bg
    GUI_DrawFillRectangle(name_listview_x, name_listview_y, name_listview_w, name_listview_h, GUI_DEF_COLOR_BG);
    // Draw preview bg
    GUI_DrawFillRectangle(preview_imageview_x, preview_imageview_y, preview_imageview_w, preview_imageview_h, GUI_DEF_COLOR_BG);

    // Draw path
    x = path_textview_x + PATH_VIEW_PADDING_L;
    y = path_textview_y + PATH_VIEW_PADDING_T;
    clip_w = path_textview_w - PATH_VIEW_PADDING_L * 2;
    clip_h = path_textview_h;
    GUI_EnableClipping();
    GUI_SetClipRectangle(x, path_textview_y, clip_w, clip_h);
    GUI_DrawText(x, y, PATH_TEXT_COLOR, l_data->path);
    GUI_DisableClipping();

    // Draw FileListEntries
    LinkedListEntry *f_entry = LinkedListFind(file_list, listview_top_pos);

    if (f_entry)
    {
        int itemview_max_dy = name_listview_y + name_listview_h - NAME_LISTVIEW_PADDING_T;
        int itemview_x = name_listview_x + NAME_LISTVIEW_PADDING_L;
        int itemview_y = name_listview_y + NAME_LISTVIEW_PADDING_T;
        uint32_t color;

        int i;
        for (i = listview_top_pos; i < l_length; i++)
        {
            FileListEntryData *e_data = (FileListEntryData *)LinkedListGetEntryData(f_entry);
            if (!e_data)
                continue;

            if (itemview_y >= itemview_max_dy)
                break;

            if (e_data->is_folder)
                color = NAME_TEXT_COLOR_FOLDER;
            else
                color = NAME_TEXT_COLOR_FILE;

            GUI_EnableClipping();

            clip_h = name_itemview_h;
            if (clip_h > itemview_max_dy - itemview_y)
                clip_h = itemview_max_dy - itemview_y;

            if (i == listview_focus_pos)
            {
                clip_w = name_itemview_w;
                GUI_SetClipRectangle(itemview_x, itemview_y, clip_w, clip_h);
                GUI_DrawFillRectangle(itemview_x, itemview_y, name_itemview_w, name_itemview_h, NAME_ITEMVIEW_COLOR_FOCUS_BG);

                // Name scroll
                clip_w = name_itemview_w - NAME_ITEMVIEW_PADDING_L * 2;
                w = GUI_GetTextWidth(e_data->name);
                if (w > clip_w)
                {
                    name_scroll_count++;
                    if (name_scroll_count > 60) // Start scroll, the delay is 60 frames
                    {
                        int effect_count = name_scroll_count - 60;
                        if (effect_count < w) // Enable scroll, set the coord x offset
                            x -= effect_count;
                        else if (effect_count < w + 30) // No scroll, fade-in in 30 frames
                            color = (color & 0x00FFFFFF) | ((((color >> 24) * (effect_count - w)) / 30) << 24);
                        else // No scroll, reset the scroll count for loop
                            name_scroll_count = 0;
                    }
                }
            }

            x = itemview_x + NAME_ITEMVIEW_PADDING_L;
            y = itemview_y + NAME_ITEMVIEW_PADDING_T;
            clip_w = name_itemview_w - NAME_ITEMVIEW_PADDING_L * 2;
            GUI_SetClipRectangle(x, itemview_y, clip_w, clip_h);
            GUI_DrawText(x, y, color, e_data->name);

            GUI_DisableClipping();

            f_entry = LinkedListNext(f_entry);
            itemview_y += name_itemview_h;
        }

        GUI_DrawVerticalScrollbar(name_scrollbar_track_x, name_scrollbar_track_y, name_scrollbar_track_height, l_length, name_listview_n_draw_items, listview_top_pos, 0);

        if (preview_tex)
            GUI_DrawTextureScale(preview_tex, preview_tex_x, preview_tex_y, preview_tex_x_scale, preview_tex_y_scale);
    }
}

static void deleteGameCallback(GUI_Dialog *dialog)
{
    char path[MAX_PATH_LENGTH];
    if (MakeCurrentFilePath(path) < 0)
        return;

    sceIoRemove(path);
#ifdef FBA_BUILD
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    char base_name[MAX_NAME_LENGTH];
    MakeBaseName(base_name, path, MAX_NAME_LENGTH);
    snprintf(path, MAX_PATH_LENGTH, "%s%s.dat", l_data->path, base_name);
    sceIoRemove(path);
#endif
    GUI_CloseDialog(dialog->prev);
    AlertDialog_Dismiss(dialog);
    refreshFileList(file_list);
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

    AlertDialogData *data = (AlertDialogData *)dialog->userdata;
    switch (data->focus_pos)
    {
    case INDEX_OPTION_ITEM_LOAD_GAME:
    {
        LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
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
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
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
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
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
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
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
    name_scroll_count = 0;
    LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (!data || data->is_folder)
        return;

    GUI_Dialog *dialog = AlertDialog_Creat();
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
    name_scroll_count = 0;
    LinkedListEntry *entry = LinkedListFind(file_list, listview_focus_pos);
    FileListEntryData *data = (FileListEntryData *)LinkedListGetEntryData(entry);
    if (data)
    {
        if (data->is_folder)
            enterChildDir(entry);
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
        Browser_ChangeDirByFile(LASTFILE_PATH);
    }
}

static int startActivityCallback(GUI_Activity *activity)
{
    browser_activity.wallpaper = GUI_GetDefaultWallpaper();
    refreshLayout();

    if (!file_list)
        file_list = FileListCreate();
    if (!file_list)
        return -1;
    FileListData *l_data = (FileListData *)LinkedListGetListData(file_list);
    strcpy(l_data->path, HOME_PATH);
    refreshFileList();
    Browser_ChangeDirByFile(LASTFILE_PATH);

    return 0;
}

static int exitActivityCallback(GUI_Activity *activity)
{
    LinkedListEmpty(file_list);
    return 0;
}
