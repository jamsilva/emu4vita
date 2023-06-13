#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/io/fcntl.h>

#include "List/file_list.h"
#include "Setting/setting.h"
#include "Retro/retro.h"
#include "Emu/emu.h"
#include "Gui/gui.h"
#include "browser.h"
#include "utils.h"
#include "file.h"
#include "config.h"
#include "lang.h"

extern GUI_Activity about_activity;

static int enterActivityCallback(GUI_Activity *activity);
static int exitActivityCallback(GUI_Activity *activity);
static void drawActivityCallback(GUI_Activity *activity);
static void ctrlActivityCallback(GUI_Activity *activity);

static GUI_ButtonInstruction button_instructions[] = {
    {BUTTON_CANCEL, PARENT_DIRECTORY},
    {BUTTON_ENTER, OPEN},
    {BUTTON_TRIANGLE, OPTION_MENU},
    {BUTTON_PSBUTTON, EMU_SETTING},
    {BUTTON_SELECT, ABOUT},
    {BUTTON_START, CHANGE_DIRECTORY},
    {LANG_DISABLE, LANG_DISABLE},
};

GUI_Activity browser_activity = {
    APP_TITLE,             // Title
    button_instructions,   // Button instructions
    enterActivityCallback, // Init callback
    exitActivityCallback,  // Exit callback
    drawActivityCallback,  // Draw callback
    ctrlActivityCallback,  // Ctrl callback
    0,                     // Disable draw statusbar
    0,                     // Disable draw wallpaper
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
    START_GAME,
    DELETE_GAME,
    DELETE_AUTO_STATE,
    DELETE_SAVEFILE,
};
#define N_OPTION_ITEMS (sizeof(option_items) / sizeof(int))

#define MAX_DIR_LEVELS 128

#define LAYOUT_CHILD_MARGIN 2.0f

#define PATH_VIEW_PADDING_L 10.0f
#define PATH_VIEW_PADDING_T 6.0f
#define PATH_TEXT_COLOR COLOR_SPRING_GREEN

#define NAME_LISTVIEW_PADDING_L 2.0f
#define NAME_LISTVIEW_PADDING_T 2.0f
#define NAME_ITEMVIEW_PADDING_L 8.0f
#define NAME_ITEMVIEW_PADDING_T 5.0f
#define NAME_TEXT_COLOR_FOLDER COLOR_WHITE
#define NAME_TEXT_COLOR_FILE COLOR_GREEN
#define NAME_ITEMVIEW_COLOR_FOCUS_BG GUI_DEFALUT_COLOR_FOCUS_BG

#define PREVIEW_VIEW_PADDING 8.0f
#define PREVIEW_REFRESH_DELAY_FRAMES 6

// File list
static FileList file_list = {0};
static int list_sort_mode = SORT_BY_NAME;

// Position
static int list_top_pos = 0, list_focus_pos = 0;
static int top_pos_saves[MAX_DIR_LEVELS] = {0}, focus_pos_saves[MAX_DIR_LEVELS] = {0};
static int dir_level = 0;

// Preview
static GUI_Texture *preview_tex = NULL;
static int preview_need_refresh = 1;
static int preview_refresh_delay_frames = 0;

// Coord
static int path_view_width, path_view_height;
static int path_view_sx, path_view_sy, path_view_dx, path_view_dy;

static int name_listview_width, name_listview_height;
static int name_listview_sx, name_listview_sy, name_listview_dx, name_listview_dy;
static int name_itemview_width, name_itemview_height;
static int name_scroll_count = 0;
static int list_max_draw_len;

static int scrollbar_track_x, scrollbar_track_y;
static int scrollbar_track_height;

static int preview_view_sx, preview_view_sy, preview_view_dx, preview_view_dy;
static int preview_view_width, preview_view_height;
static int preview_tex_sx, preview_tex_sy;
static float preview_tex_x_scale, preview_tex_y_scale;

static void refreshLayout()
{
    path_view_width = ACTIVITY_FREE_DRAW_WIDTH;
    path_view_height = GUI_GetFontSize() + PATH_VIEW_PADDING_T * 2;
    path_view_sx = ACTIVITY_FREE_DRAW_SX;
    path_view_sy = ACTIVITY_FREE_DRAW_SY;
    path_view_dx = path_view_sx + path_view_width;
    path_view_dy = path_view_sy + path_view_height;

    preview_view_sy = path_view_dy + LAYOUT_CHILD_MARGIN;
    preview_view_dy = ACTIVITY_FREE_DRAW_DY;
    preview_view_height = preview_view_dy - preview_view_sy;
    preview_view_width = preview_view_height;
    preview_view_dx = ACTIVITY_FREE_DRAW_DX;
    preview_view_sx = preview_view_dx - preview_view_width;

    name_listview_sx = path_view_sx;
    name_listview_sy = preview_view_sy;
    name_listview_dx = preview_view_sx - LAYOUT_CHILD_MARGIN;
    name_listview_dy = preview_view_dy;
    name_listview_width = name_listview_dx - name_listview_sx;
    name_listview_height = name_listview_dy - name_listview_sy;

    name_itemview_width = name_listview_width - NAME_LISTVIEW_PADDING_L * 2;
    name_itemview_height = GUI_GetFontSize() + NAME_ITEMVIEW_PADDING_T * 2;
    list_max_draw_len = (name_listview_height - NAME_LISTVIEW_PADDING_T * 2) / name_itemview_height;

    scrollbar_track_x = name_listview_dx - GUI_SCROLLBAR_SIZE - 2;
    scrollbar_track_y = name_listview_sy + 2;
    scrollbar_track_height = name_listview_height - 4;
}

int CurrentPathIsFile()
{
    FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
    if (!entry || entry->is_folder)
        return 0;

    return 1;
}

int MakeCurrentFileName(char *name)
{
    FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
    if (!entry || !entry->name)
        return -1;

    snprintf(name, MAX_NAME_LENGTH, entry->name);
    return 0;
}

int MakeCurrentFilePath(char *path)
{
    FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
    if (!entry || !entry->name)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, "%s%s", file_list.path, entry->name);
    return 0;
}

int MakePreviewPath(char *path)
{
    FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
    if (!entry || !entry->name)
        return -1;

    char base_name[MAX_NAME_LENGTH];
    int ret = MakeBaseName(base_name, entry->name, MAX_NAME_LENGTH);
    if (ret < 0)
        return -1;

    snprintf(path, MAX_PATH_LENGTH, ("%s/%s/%s.png"), file_list.path, PREVIEW_DIR_NAME, base_name);
    return 0;
}

int MakeScreenshotPath(char *path)
{
    FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
    if (!entry || !entry->name)
        return -1;

    char base_name[MAX_NAME_LENGTH];
    int ret = MakeBaseName(base_name, entry->name, MAX_NAME_LENGTH);
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
    return GUI_LoadPNGFile(path);
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
        GUI_WaitRenderDone();
        GUI_DestroyTexture(preview_tex);
        preview_tex = NULL;
    }
}

static void refreshPreview()
{
    if (preview_tex)
    {
        GUI_WaitRenderDone();
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

    if (!preview_tex)
        return;

    int tex_width = GUI_GetTextureWidth(preview_tex);
    int tex_height = GUI_GetTextureHeight(preview_tex);
    int max_width = preview_view_width - PREVIEW_VIEW_PADDING * 2;
    int max_height = preview_view_height - PREVIEW_VIEW_PADDING * 2;
    int width = max_width;
    int height = max_height;
    if (app_config.preview_style == TYPE_PREVIEW_STYLE_PRESERVE_FULL)
    {
        float ratio = (float)tex_width / (float)tex_height;
        height = width / ratio;
        if (height > max_height)
        {
            height = max_height;
            width = height * ratio;
        }
    }
    preview_tex_sx = preview_view_sx + PREVIEW_VIEW_PADDING + (max_width - width) / 2;
    preview_tex_sy = preview_view_sy + PREVIEW_VIEW_PADDING + (max_height - height) / 2;
    preview_tex_x_scale = (float)width / (float)tex_width;
    preview_tex_y_scale = (float)height / (float)tex_height;
}

static void moveFileListPos(int type)
{
    static int old_dir_level = -1;
    static int old_focus_pos = -1;

    MoveListPos(type, &list_top_pos, &list_focus_pos, file_list.length, list_max_draw_len);
    if (old_dir_level != dir_level || old_focus_pos != list_focus_pos)
    {
        name_scroll_count = 0;
        Browser_RequestRefreshPreview(0);
        Settting_SetStateSelectId(0);
    }
    old_dir_level = dir_level;
    old_focus_pos = list_focus_pos;
}

static void dirLevelUp()
{
    if (dir_level < MAX_DIR_LEVELS - 1)
    {
        top_pos_saves[dir_level] = list_top_pos;
        focus_pos_saves[dir_level] = list_focus_pos;
        dir_level++;
    }

    list_top_pos = 0;
    list_focus_pos = 0;
}

static void dirLevelDown()
{
    RemoveEndSlash(file_list.path);

    char *p;
    p = strrchr(file_list.path, '/'); // ux0:abc/cde
    if (p)
    {
        p[1] = '\0';
        dir_level--;
        goto DIR_UP_RETURN;
    }

    p = strrchr(file_list.path, ':'); // ux0:abc || ux0:
    if (p)
    {
        if (strlen(file_list.path) - ((p + 1) - file_list.path) > 0) // ux0:abc
        {
            p[1] = '\0';
            dir_level--;
            goto DIR_UP_RETURN;
        }
    }

    strcpy(file_list.path, HOME_PATH);
    dir_level = 0;

DIR_UP_RETURN:
    if (dir_level < 0)
        dir_level = 0;

    list_top_pos = top_pos_saves[dir_level];
    list_focus_pos = focus_pos_saves[dir_level];
}

static int setFocusByName(const char *name)
{
    int pos = FileListGetNumberByName(&file_list, name);
    if (pos < 0 || pos >= file_list.length)
        return -1;

    list_focus_pos = pos;
    moveFileListPos(TYPE_MOVE_NONE);
    return 0;
}

static int refreshFileList()
{
    int ret = 0, res = 0;

    do
    {
        FileListEmpty(&file_list);
        res = FileListGetEntries(&file_list, file_list.path, list_sort_mode);
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

    dir_level = 0;
    strcpy(file_list.path, HOME_PATH);

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

            strcpy(file_list.path, lastdir);

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

static void handleFile(FileListEntry *file_entry)
{
    char path[MAX_PATH_LENGTH];
    snprintf(path, MAX_PATH_LENGTH, "%s%s", file_list.path, file_entry->name);

    if (Emu_LoadGame(path) < 0)
        return;
    if (misc_config.auto_save_load)
        Emu_LoadState(-1);
}

static void handleFolder(FileListEntry *file_entry)
{
    if (dir_level == 0)
    {
        strcpy(file_list.path, file_entry->name);
    }
    else
    {
        if (dir_level > 1)
            AddEndSlash(file_list.path);
        strcat(file_list.path, file_entry->name);
    }

    dirLevelUp();
    refreshFileList();
}

static void checkPreview()
{
    if (preview_need_refresh)
    {
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
}

static void drawActivityCallback(GUI_Activity *activity)
{
    int sx, sy;
    int width, height;

    checkPreview();

    // Draw path bg
    GUI_DrawFillRectangle(path_view_sx, path_view_sy, path_view_width, path_view_height, GUI_DEFALUT_BG_COLOR);
    // Draw name bg
    GUI_DrawFillRectangle(name_listview_sx, name_listview_sy, name_listview_width, name_listview_height, GUI_DEFALUT_BG_COLOR);
    // Draw preview bg
    GUI_DrawFillRectangle(preview_view_sx, preview_view_sy, preview_view_width, preview_view_height, GUI_DEFALUT_BG_COLOR);

    // Draw path
    sx = path_view_sx + PATH_VIEW_PADDING_L;
    sy = path_view_sy + PATH_VIEW_PADDING_T;
    width = path_view_width - PATH_VIEW_PADDING_L * 2;
    height = path_view_height - PATH_VIEW_PADDING_T * 2;
    GUI_EnableClipping();
    GUI_SetClipRectangle(sx, sy, width, height);
    GUI_DrawText(sx, sy, PATH_TEXT_COLOR, file_list.path);
    GUI_DisableClipping();

    // Draw FileListEntries
    FileListEntry *file_entry = FileListGetEntryByNumber(&file_list, list_top_pos);

    if (file_entry)
    {
        int item_sx = name_listview_sx + NAME_LISTVIEW_PADDING_L;
        int item_sy = name_listview_sy + NAME_LISTVIEW_PADDING_T;
        int text_sx = item_sx + NAME_ITEMVIEW_PADDING_L;
        int text_width = name_itemview_width - NAME_ITEMVIEW_PADDING_L * 2;
        int text_height = name_itemview_height - NAME_ITEMVIEW_PADDING_T * 2;
        int drawn_len = 0;

        int i;
        for (i = list_top_pos; i < file_list.length; i++)
        {
            if (drawn_len >= list_max_draw_len)
                break;

            sx = text_sx;
            sy = item_sy + NAME_ITEMVIEW_PADDING_T;

            uint32_t color;
            if (file_entry->is_folder)
                color = NAME_TEXT_COLOR_FOLDER;
            else
                color = NAME_TEXT_COLOR_FILE;

            if (i == list_focus_pos)
            {
                GUI_DrawFillRectangle(item_sx, item_sy, name_itemview_width, name_itemview_height, NAME_ITEMVIEW_COLOR_FOCUS_BG);

                width = GUI_GetTextWidth(file_entry->name);
                if (width > text_width)
                {
                    name_scroll_count++;
                    if (name_scroll_count > 60) // Start scroll, the delay is 60 frames
                    {
                        int effect_count = name_scroll_count - 60;
                        if (effect_count < width) // Enable scroll, set the coord x offset
                            sx -= effect_count;
                        else if (effect_count < width + 30) // No scroll, fade-in in 30 frames
                            color = (color & 0x00FFFFFF) | ((((color >> 24) * (effect_count - width)) / 30) << 24);
                        else // No scroll, reset the scroll count for loop
                            name_scroll_count = 0;
                    }
                }
            }

            GUI_EnableClipping();
            GUI_SetClipRectangle(text_sx, sy, text_width, text_height);
            GUI_DrawText(sx, sy, color, file_entry->name);
            GUI_DisableClipping();

            file_entry = file_entry->next;
            item_sy += name_itemview_height;
            drawn_len++;
        }

        GUI_DrawVerticalScrollbar(scrollbar_track_x, scrollbar_track_y, scrollbar_track_height, file_list.length, list_max_draw_len, list_top_pos, 0);

        if (preview_tex)
            GUI_DrawTextureScale(preview_tex, preview_tex_sx, preview_tex_sy, preview_tex_x_scale, preview_tex_y_scale);
    }
}

static void deleteGameCallback(GUI_Dialog *dialog)
{
    char path[MAX_PATH_LENGTH];
    if (MakeCurrentFilePath(path) < 0)
        return;
    sceIoRemove(path);
    GUI_CloseAllDialogs();
    refreshFileList(&file_list);
    Browser_RequestRefreshPreview(0);
}

static void deleteAutoStateCallback(GUI_Dialog *dialog)
{
    Emu_DeleteState(-1);
    GUI_CloseAllDialogs();
    Browser_RequestRefreshPreview(0);
}

static void deleteSrmCallback(GUI_Dialog *dialog)
{
    Emu_DeleteSrm();
    GUI_CloseAllDialogs();
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
        FileListEntry *entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
        if (entry && !entry->is_folder)
        {
            GUI_CloseAllDialogs();
            handleFile(entry);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_GAME:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[TIP_DELETE_GAME]);
            AlertDialog_SetPositiveCallback(tip_dialog, cur_lang[CONFIRM], deleteGameCallback);
            AlertDialog_SetNegativeCallback(tip_dialog, cur_lang[CANCEL], NULL);
            GUI_OpenDialog(tip_dialog);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_AUTO_STATE:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[TIP_DELETE_AUTO_STATE]);
            AlertDialog_SetPositiveCallback(tip_dialog, cur_lang[CONFIRM], deleteAutoStateCallback);
            AlertDialog_SetNegativeCallback(tip_dialog, cur_lang[CANCEL], NULL);
            GUI_OpenDialog(tip_dialog);
        }
    }
    break;
    case INDEX_OPTION_ITEM_DELETE_SAVEFILE:
    {
        if (CurrentPathIsFile())
        {
            GUI_Dialog *tip_dialog = AlertDialog_Creat();
            AlertDialog_SetTitle(tip_dialog, cur_lang[TIP]);
            AlertDialog_SetMessage(tip_dialog, cur_lang[TIP_DELETE_SAVEFILE]);
            AlertDialog_SetPositiveCallback(tip_dialog, cur_lang[CONFIRM], deleteSrmCallback);
            AlertDialog_SetNegativeCallback(tip_dialog, cur_lang[CANCEL], NULL);
            GUI_OpenDialog(tip_dialog);
        }
    }
    break;
    default:
        break;
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

    if (pressed_pad[PAD_TRIANGLE])
    {
        GUI_Dialog *dialog = AlertDialog_Creat();
        AlertDialog_SetTitle(dialog, cur_lang[MENU]);
        int list_len = N_OPTION_ITEMS;
        char **list = GUI_GetStringListByLangIdList(option_items, list_len);
        AlertDialog_SetMenu(dialog, list, list_len);
        AlertDialog_SetPositiveCallback(dialog, cur_lang[CONFIRM], optionMenuPositiveCallback);
        AlertDialog_SetNegativeCallback(dialog, cur_lang[CANCEL], NULL);
        GUI_OpenDialog(dialog);
    }
    else if (pressed_pad[PAD_CANCEL])
    {
        if (dir_level > 0)
        {
            dirLevelDown();
            refreshFileList();
        }
    }
    else if (pressed_pad[PAD_ENTER])
    {
        name_scroll_count = 0;
        FileListEntry *file_entry = FileListGetEntryByNumber(&file_list, list_focus_pos);
        if (file_entry)
        {
            if (file_entry->is_folder)
                handleFolder(file_entry);
            else
                handleFile(file_entry);
        }
    }
    else if (pressed_pad[PAD_SELECT])
    {
        GUI_EnterActivity(&about_activity);
    }
    else if (pressed_pad[PAD_START])
    {
        Browser_ChangeDirByFile(LASTFILE_PATH);
    }
}

static int enterActivityCallback(GUI_Activity *activity)
{
    refreshLayout();

    FileListEmpty(&file_list);
    strcpy(file_list.path, HOME_PATH);
    refreshFileList();
    Browser_ChangeDirByFile(LASTFILE_PATH);

    return 0;
}

static int exitActivityCallback(GUI_Activity *activity)
{
    FileListEmpty(&file_list);
    return 0;
}
