#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <psp2/kernel/threadmgr.h>

#include <vita2d.h>

#include "browser.h"
#include "ui.h"
#include "main.h"
#include "utils.h"
#include "file.h"
#include "config.h"
#include "lang.h"
#include "boot.h"

#define VIEW_MARGIN 10.0f

#define ICON_WIDTH 128.0f
#define ICON_HEIGHT 128.0f

#define ICON_INVERTED_ALPHA 0x2F
#define ICON_INVERTED_COLOR COLOR_ALPHA(WHITE, ICON_INVERTED_ALPHA)

#define ICON_COLOR_NONE WHITE
#define ICON_INVERTED_COLOR_NONE COLOR_ALPHA(WHITE, ICON_INVERTED_ALPHA)

#define ICON_UNFOCUS_SCALE 0.9f
#define ICON_FOCUS_SCALE 1.05f

#define CORE_VIEW_PADDING_L 30.0f
#define CORE_VIEW_PADDING_T 40.0f

#define CORE_TITLE_PADDING_L 10.0f
#define CORE_TITLE_PADDING_T 10.0f

#define CORE_ENTRIES_PADDING_L 10.0f
#define CORE_ENTRIES_PADDING_T 10.0f
#define CORE_ENTRIES_MARGIN_L 60.0f
#define CORE_ENTRIES_LINE_SPACE 2.0f

#define CORE_TITLE_COLOR SPRING_GREEN

CoreEntry nes_entries[] = {
    {"FCEUmm", "fceumm"},
    {"Nestopia", "nestopia"},
};

CoreEntry snes_entries[] = {
    {"Snes9x 2002", "snes9x2002"},
    {"Snes9x 2005", "snes9x2005"},
    {"Snes9x 2005 Plus", "snes9x2005_plus"},
};

CoreEntry md_entries[] = {
    {"Genesis Plus GX", "genesis_plus_gx"},
    {"PicoDrive", "picodrive"},
};

CoreEntry gba_entries[] = {
    {"gpSP", "gpsp"},
    {"VBA Next", "vba_next"},
};

CoreEntry gbc_entries[] = {
    {"Gambatte", "gambatte"},
};

CoreEntry pce_entries[] = {
    {"Mednafen PCE Fast", "mednafen_pce_fast"},
    {"Mednafen SuperGrafx", "mednafen_supergrafx"},
};

CoreEntry ps_entries[] = {
    {"PCSX ReARMed", "pcsx_rearmed"},
};

CoreEntry wsc_entries[] = {
    {"Mednafen Wswan", "mednafen_wswan"},
};

CoreEntry ngp_entries[] = {
    {"Mednafen NeoPop", "mednafen_ngp"},
};

CoreEntry fba_entries[] = {
    {"FBA Lite", "fba_lite"},
};

BrowserEntry browser_entries[] = {
    {"NES", NULL, nes_entries, sizeof(nes_entries) / sizeof(CoreEntry)},
    {"SNES", NULL, snes_entries, sizeof(snes_entries) / sizeof(CoreEntry)},
    {"MD", NULL, md_entries, sizeof(md_entries) / sizeof(CoreEntry)},
    {"GBA", NULL, gba_entries, sizeof(gba_entries) / sizeof(CoreEntry)},
    {"GBC", NULL, gbc_entries, sizeof(gbc_entries) / sizeof(CoreEntry)},
    {"PCE", NULL, pce_entries, sizeof(pce_entries) / sizeof(CoreEntry)},
    {"PS1", NULL, ps_entries, sizeof(ps_entries) / sizeof(CoreEntry)},
    {"WSC", NULL, wsc_entries, sizeof(wsc_entries) / sizeof(CoreEntry)},
    {"NGP", NULL, ngp_entries, sizeof(ngp_entries) / sizeof(CoreEntry)},
    {"FBA", NULL, fba_entries, sizeof(ngp_entries) / sizeof(CoreEntry)},
};
#define N_BROWSER_ENTRIES (sizeof(browser_entries) / sizeof(BrowserEntry))

CoreEntry *core_entries = NULL;

static float icon_sx, icon_middle_y;

static float icon_focus_sx, icon_focus_scroll_sx, icon_focus_scroll_sy;
static float icon_focus_min_sy, icon_focus_max_sy;
static float icon_focus_width, icon_focus_height, icon_focus_margin;

static float icon_unfocus_sy;
static float icon_unfocus_width, icon_unfocus_height, icon_unfocus_margin;

static float core_title_view_sx, core_title_view_sy, core_title_view_dx, core_title_view_scroll_sx;
static float core_title_view_width, core_title_view_height;
static float core_title_text_sx, core_title_text_sy, core_title_text_dx, core_title_text_scroll_sx;
static float core_title_text_width, core_title_text_height;

static float core_entries_view_sx, core_entries_view_sy, core_entries_view_dx, core_entries_view_scroll_sx;
static float core_entries_view_width, core_entries_view_height;
static float core_entries_text_sx, core_entries_text_sy, core_entries_text_dx, core_entries_text_scroll_sx;
static float core_entries_text_width, core_entries_text_height;

static float core_entries_full_height;
static float core_entries_y_space;

static float core_open_icon_sx, core_open_icon_sy;
static float core_open_title_view_sx, core_open_title_view_sy;
static float core_open_title_text_sx, core_open_title_text_sy;
static float core_open_entries_view_sx, core_open_entries_view_sy;
static float core_open_entries_text_sx, core_open_entries_text_sy;

static int core_enties_lines;

static int browser_top_pos = 0, browser_focus_pos = 0;
static int core_top_pos = 0, core_focus_pos = 0;
static int core_open = 0;

void initBrowserDrawInfo()
{
    icon_unfocus_width = ICON_WIDTH * ICON_UNFOCUS_SCALE;
    icon_unfocus_height = ICON_HEIGHT * ICON_UNFOCUS_SCALE;
    icon_unfocus_margin = VIEW_MARGIN * ICON_UNFOCUS_SCALE;

    icon_focus_width = ICON_WIDTH * ICON_FOCUS_SCALE;
    icon_focus_height = ICON_HEIGHT * ICON_FOCUS_SCALE;
    icon_focus_margin = VIEW_MARGIN * ICON_FOCUS_SCALE;

    float icon_focus_margin_b = icon_unfocus_height / 3;

    icon_sx = 0 - icon_unfocus_width / 4;
    icon_focus_sx = icon_sx + (icon_unfocus_width + icon_focus_margin);

    float icon_focus_max_inverted_dy = MAIN_FREE_DRAW_DY - icon_focus_margin;
    float icon_focus_max_inverted_sy = icon_focus_max_inverted_dy - icon_focus_height;

    icon_middle_y = icon_focus_max_inverted_sy - icon_focus_margin_b - icon_focus_margin / 2;

    float icon_focus_min_dy = icon_middle_y - icon_focus_margin / 2;
    float icon_focus_max_dy = icon_focus_min_dy - icon_focus_margin_b;
    icon_focus_min_sy = icon_focus_min_dy - icon_focus_height;
    icon_focus_max_sy = icon_focus_max_dy - icon_focus_height;

    icon_focus_scroll_sx = icon_focus_sx;
    icon_focus_scroll_sy = icon_focus_min_sy;

    float icon_unfocus_dy = icon_middle_y - icon_unfocus_margin / 2;
    icon_unfocus_sy = icon_unfocus_dy - icon_unfocus_height;

    core_title_view_height = UiGetLineHeight() + CORE_TITLE_PADDING_T * 2;
    core_title_view_sx = icon_focus_sx + icon_focus_width + icon_focus_margin;
    core_title_view_sy = icon_unfocus_sy - VIEW_MARGIN - core_title_view_height;
    core_title_view_dx = MAIN_FREE_DRAW_DX - CORE_VIEW_PADDING_L;
    core_title_view_scroll_sx = core_title_view_sx;
    core_title_view_width = core_title_view_dx - core_title_view_sx;

    core_title_text_sx = core_title_view_sx + CORE_TITLE_PADDING_L;
    core_title_text_sy = core_title_view_sy + CORE_TITLE_PADDING_T;
    core_title_text_dx = core_title_view_dx - CORE_TITLE_PADDING_L;
    core_title_text_scroll_sx = core_title_text_sx;
    core_title_text_width = core_title_view_width - CORE_TITLE_PADDING_L * 2;
    core_title_text_height = UiGetLineHeight();

    core_entries_view_sx = core_title_view_sx + CORE_ENTRIES_MARGIN_L + 4;
    core_entries_view_sy = core_title_view_sy + core_title_view_height;
    core_entries_view_dx = core_title_view_dx;
    core_entries_view_scroll_sx = core_entries_view_sx;
    core_entries_view_width = core_entries_view_dx - core_entries_view_sx;
    core_entries_view_height = UiGetLineHeight() * CORE_ENTRIES_PADDING_T * 2;

    core_entries_text_sx = core_entries_view_sx + CORE_ENTRIES_PADDING_L;
    core_entries_text_sy = core_entries_view_sy + CORE_ENTRIES_PADDING_T;
    core_entries_text_dx = core_title_text_dx;
    core_entries_text_scroll_sx = core_entries_text_sx;
    core_entries_text_width = core_entries_text_dx - core_entries_text_sx;
    core_entries_text_height = UiGetLineHeight();

    core_entries_y_space = UiGetLineHeight() + CORE_ENTRIES_PADDING_T + CORE_ENTRIES_LINE_SPACE;
    core_entries_full_height = MAIN_FREE_DRAW_DY - CORE_VIEW_PADDING_T - core_entries_text_sy;
    core_enties_lines = (core_entries_full_height + CORE_ENTRIES_LINE_SPACE) / core_entries_y_space;

    core_open_icon_sx = CORE_VIEW_PADDING_L;
    core_open_icon_sy = icon_focus_min_sy;

    core_open_title_view_sx = core_open_icon_sx + icon_focus_width + icon_focus_margin;
    core_open_title_view_sy = core_title_view_sy;

    core_open_title_text_sx = core_open_title_view_sx + CORE_TITLE_PADDING_L;
    core_open_title_text_sy = core_title_text_sy;

    core_open_entries_view_sx = core_open_title_view_sx + CORE_ENTRIES_MARGIN_L;
    core_open_entries_view_sy = core_entries_view_sy;

    core_open_entries_text_sx = core_open_entries_view_sx + CORE_ENTRIES_PADDING_L;
    core_open_entries_text_sy = core_entries_text_sy;
}

static void makeCorePath(char *path, char *name)
{
    snprintf(path, MAX_PATH_LENGTH, "app0:%s.self", name);
}

static void makeIconPath(char *path, char *name)
{
    snprintf(path, MAX_PATH_LENGTH, "%s/%s/icon0.png", CORE_DATA_DIR, name);
}

static void setIconTexture(BrowserEntry *entry, vita2d_texture *texture)
{
    if (entry->icon)
    {
        vita2d_wait_rendering_done();
        vita2d_free_texture(entry->icon);
    }
    entry->icon = texture;
}

static int loadCoreEboot(CoreEntry *entry)
{
    if (!entry)
        return -1;

    char path[MAX_PATH_LENGTH];
    char *name = entry->core_name;
    makeCorePath(path, name);
    char *software = browser_entries[browser_focus_pos].software;
    WriteFile(LASTFILE_PATH, software, strlen(software) + 1);
    int ret = loadCoreWithBootParams(path, software);

    return ret;
}

static void refreshBrowserEntriesPos(int type, int pos)
{
    int tmp_focus_pos = pos;
    if (type == LIST_CONTROL_UP)
        tmp_focus_pos--;
    else if (type == LIST_CONTROL_DOWN)
        tmp_focus_pos++;

    if (tmp_focus_pos < 0)
        tmp_focus_pos = 0;
    if (tmp_focus_pos > N_BROWSER_ENTRIES - 1)
        tmp_focus_pos = N_BROWSER_ENTRIES - 1;

    if (browser_focus_pos != tmp_focus_pos)
    {
        browser_focus_pos = tmp_focus_pos;
        int tmp_top_pos = browser_focus_pos - 1;
        if (tmp_top_pos < 0)
            tmp_top_pos = 0;
        browser_top_pos = tmp_top_pos;
        icon_focus_scroll_sy = icon_focus_min_sy;
    }
}

static void refreshCoreEntriesPos(int type, int pos)
{
    int length = browser_entries[browser_focus_pos].n_entries;
    controlRefreshListPos(type, &core_top_pos, &pos, length, core_enties_lines);
    if (core_focus_pos != pos)
    {
        core_focus_pos = pos;
    }
}

static void refreshBrowserScroll()
{
    if (icon_focus_scroll_sx < icon_focus_sx)
        icon_focus_scroll_sx += 4;
    if (icon_focus_scroll_sx > icon_focus_sx)
        icon_focus_scroll_sx = icon_focus_sx;

    if (icon_focus_scroll_sy > icon_focus_max_sy)
        icon_focus_scroll_sy -= 4;
    if (icon_focus_scroll_sy < icon_focus_max_sy)
        icon_focus_scroll_sy = icon_focus_max_sy;

    if (core_title_view_scroll_sx < core_title_view_sx)
        core_title_view_scroll_sx += 4;
    if (core_title_view_scroll_sx > core_title_view_sx)
        core_title_view_scroll_sx = core_title_view_sx;

    if (core_title_text_scroll_sx < core_title_text_sx)
        core_title_text_scroll_sx += 4;
    if (core_title_text_scroll_sx > core_title_text_sx)
        core_title_text_scroll_sx = core_title_text_sx;

    if (core_entries_view_scroll_sx < core_entries_view_sx)
        core_entries_view_scroll_sx += 4;
    if (core_entries_view_scroll_sx > core_entries_view_sx)
        core_entries_view_scroll_sx = core_entries_view_sx;

    if (core_entries_text_scroll_sx < core_entries_text_sx)
        core_entries_text_scroll_sx += 4;
    if (core_entries_text_scroll_sx > core_entries_text_sx)
        core_entries_text_scroll_sx = core_entries_text_sx;
}

static void refreshCoreScroll()
{
    if (icon_focus_scroll_sx > core_open_icon_sx)
        icon_focus_scroll_sx -= 4;
    if (icon_focus_scroll_sx < core_open_icon_sx)
        icon_focus_scroll_sx = core_open_icon_sx;

    if (icon_focus_scroll_sy < core_open_icon_sy)
        icon_focus_scroll_sy += 4;
    if (icon_focus_scroll_sy > core_open_icon_sy)
        icon_focus_scroll_sy = core_open_icon_sy;

    if (core_title_view_scroll_sx > core_open_title_view_sx)
        core_title_view_scroll_sx -= 4;
    if (core_title_view_scroll_sx < core_open_title_view_sx)
        core_title_view_scroll_sx = core_open_title_view_sx;

    if (core_title_text_scroll_sx > core_open_title_text_sx)
        core_title_text_scroll_sx -= 4;
    if (core_title_text_scroll_sx < core_open_title_text_sx)
        core_title_text_scroll_sx = core_open_title_text_sx;

    if (core_entries_view_scroll_sx > core_open_entries_view_sx)
        core_entries_view_scroll_sx -= 4;
    if (core_entries_view_scroll_sx < core_open_entries_view_sx)
        core_entries_view_scroll_sx = core_open_entries_view_sx;

    if (core_entries_text_scroll_sx > core_open_entries_text_sx)
        core_entries_text_scroll_sx -= 4;
    if (core_entries_text_scroll_sx < core_open_entries_text_sx)
        core_entries_text_scroll_sx = core_open_entries_text_sx;
}

static void setBrowserFocusOnName(char *name)
{
    int i;
    for (i = 0; i < N_BROWSER_ENTRIES; i++)
    {
        if (strcmp(name, browser_entries[i].software) == 0)
        {
            if (i != browser_focus_pos)
            {
                refreshBrowserEntriesPos(LIST_CONTROL_NONE, i);
            }
            break;
        }
    }
}

static int setBrowserFocusOnNameFromFile(char *path)
{
    char name[MAX_NAME_LENGTH];
    if (ReadFile((path), name, sizeof(name)) <= 0)
        return -1;
    setBrowserFocusOnName(name);

    return 0;
}

static void openCoreEntries()
{
    core_open = 1;
    core_entries = browser_entries[browser_focus_pos].entry;
    core_focus_pos = 0;
    refreshCoreEntriesPos(LIST_CONTROL_NONE, core_focus_pos);
}

static void closeCoreEntries()
{
    core_open = 0;
    core_entries = NULL;
}

static int drawCoreEntries()
{
    refreshCoreScroll();

    BrowserEntry *b_entry = &browser_entries[browser_focus_pos];

    int inverted_sy = icon_middle_y + (icon_middle_y - icon_focus_scroll_sy - icon_focus_height);
    if (b_entry->icon)
    {
        vita2d_draw_texture_scale(b_entry->icon, icon_focus_scroll_sx, icon_focus_scroll_sy, ICON_FOCUS_SCALE, ICON_FOCUS_SCALE);
        vita2d_draw_texture_tint_scale_rotate(b_entry->icon, icon_focus_scroll_sx + icon_focus_width / 2, inverted_sy + icon_focus_height / 2,
                                              ICON_FOCUS_SCALE, ICON_FOCUS_SCALE, M_PI_2 * 2, ICON_INVERTED_COLOR);
    }
    else
    {
        vita2d_draw_rectangle(icon_focus_scroll_sx, icon_focus_scroll_sy, icon_focus_width, icon_focus_height, ICON_COLOR_NONE);
        vita2d_draw_rectangle(icon_focus_scroll_sx, inverted_sy, icon_focus_width, icon_focus_height, ICON_INVERTED_COLOR_NONE);
    }
    char *title = browser_entries[browser_focus_pos].software;
    UiDrawText(core_title_text_scroll_sx, core_title_text_sy, CORE_TITLE_COLOR, title);

    int sx = core_entries_text_scroll_sx;
    int sy = core_entries_text_sy;
    uint32_t color;

    int i;
    for (i = 0; i < b_entry->n_entries; i++)
    {
        if (i == core_focus_pos)
            color = CORE_TITLE_COLOR;
        else
            color = WHITE;
        UiDrawText(sx, sy, color, core_entries[i].desc);
        sy += core_entries_y_space;
    }

    return 0;
}

static int ctrlCoreEntries()
{
    if (hold_pad[PAD_UP] || hold2_pad[PAD_LEFT_ANALOG_UP])
    {
        refreshCoreEntriesPos(LIST_CONTROL_UP, core_focus_pos);
    }
    else if (hold_pad[PAD_DOWN] || hold2_pad[PAD_LEFT_ANALOG_DOWN])
    {
        refreshCoreEntriesPos(LIST_CONTROL_DOWN, core_focus_pos);
    }

    if (pressed_pad[PAD_CANCEL])
    {
        closeCoreEntries();
    }

    if (pressed_pad[PAD_ENTER])
    {
        loadCoreEboot(&core_entries[core_focus_pos]);
    }

    return 0;
}

static int drawBrowserEntries()
{
    refreshBrowserScroll();

    float sx = icon_sx;
    float sy = icon_unfocus_sy;
    float inverted_sy;
    float scale;
    float width, height, margin;

    int i;
    for (i = browser_top_pos; i < N_BROWSER_ENTRIES; i++)
    {
        if (sx >= MAIN_FREE_DRAW_DX)
            break;

        if (i == browser_focus_pos)
        {
            sx = icon_focus_scroll_sx;
            sy = icon_focus_scroll_sy;
            width = icon_focus_width;
            height = icon_focus_height;
            scale = ICON_FOCUS_SCALE;
        }
        else
        {
            sy = icon_unfocus_sy;
            width = icon_unfocus_width;
            height = icon_unfocus_height;
            scale = ICON_UNFOCUS_SCALE;
        }
        inverted_sy = icon_middle_y + (icon_middle_y - sy - height);

        BrowserEntry *entry = &browser_entries[i];
        if (entry->icon)
        {
            vita2d_draw_texture_scale(entry->icon, sx, sy, scale, scale);
            vita2d_draw_texture_tint_scale_rotate(entry->icon, sx + width / 2, inverted_sy + height / 2,
                                                  scale, scale, M_PI_2 * 2, ICON_INVERTED_COLOR);
        }
        else
        {
            vita2d_draw_rectangle(sx, sy, width, height, ICON_COLOR_NONE);
            vita2d_draw_rectangle(sx, inverted_sy, width, height, ICON_INVERTED_COLOR_NONE);
        }

        sx += width;
        if (i == browser_focus_pos || i + 1 == browser_focus_pos)
            sx += icon_focus_margin;
        else
            sx += icon_unfocus_margin;
    }
    char *title = browser_entries[browser_focus_pos].software;
    UiDrawText(core_title_text_scroll_sx, core_title_text_sy, CORE_TITLE_COLOR, title);

    return 0;
}

static int ctrlBrowserEntries()
{
    if (hold_pad[PAD_LEFT] || hold2_pad[PAD_LEFT_ANALOG_LEFT])
    {
        refreshBrowserEntriesPos(LIST_CONTROL_UP, browser_focus_pos);
    }
    else if (hold_pad[PAD_RIGHT] || hold2_pad[PAD_LEFT_ANALOG_RIGHT])
    {
        refreshBrowserEntriesPos(LIST_CONTROL_DOWN, browser_focus_pos);
    }

    if (pressed_pad[PAD_ENTER])
    {
        openCoreEntries();
    }

    return 0;
}

int drawBrowser()
{
    if (core_open)
        drawCoreEntries();
    else
        drawBrowserEntries();

    return 0;
}

int ctrlBrowser()
{
    if (core_open)
        ctrlCoreEntries();
    else
        ctrlBrowserEntries();

    return 0;
}

static int InitIconsThreadCallback(SceSize args, void *argp)
{
    vita2d_texture *texture;
    char path[MAX_PATH_LENGTH];

    int i;
    for (i = 0; i < N_BROWSER_ENTRIES; i++)
    {
        makeIconPath(path, browser_entries[i].software);
        texture = vita2d_load_PNG_file(path);
        setIconTexture(&browser_entries[i], texture);
    }

    sceKernelExitDeleteThread(0);
    return 0;
}

void initIconsThread()
{
    SceUID thid = sceKernelCreateThread("init_icons_thread", InitIconsThreadCallback, 0x10000100, 0x10000, 0, 0, NULL);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, NULL);
}

int initBrowser()
{
    setBrowserFocusOnNameFromFile(LASTFILE_PATH);

    return 0;
}
