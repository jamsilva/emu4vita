#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <psp2/kernel/processmgr.h>

#include "Setting/setting.h"
#include "Retro/retro.h"
#include "emu.h"
#include "utils.h"
#include "config.h"
#include "file.h"
#include "lang.h"

#define MICROS_PER_SECOND 1000000llu
#define SHOW_PLAYER_DURATION_MICROS (MICROS_PER_SECOND * 2)

static int video_okay = 0, video_pause = 1;
static int video_need_refresh = 1;

static GUI_Texture *video_texture = NULL;
static GUI_Texture *overlay_texture = NULL;
static GUI_Shader *video_shader = NULL;
static enum GuiTextureFormat old_video_texture_format = -1;
static int old_video_overlay_select = -1;

static unsigned int video_width = 0, video_height = 0;
static float video_x = 0.0f, video_y = 0.0f;
static float video_x_scale = 1.0f, video_y_scale = 1.0f;
static float video_rotate_rad = 0.0f;
static unsigned int video_frames = 0;
static float video_fps = 0.0f;

static uint64_t last_fps_micros = 0;
static uint64_t show_player_micros = 0;

void Emu_PauseVideo()
{
    video_pause = 1;
    GUI_SetVblankWait(1);
}

void Emu_ResumeVideo()
{
    video_pause = 0;
    GUI_SetVblankWait(0);
}

void Emu_RequestRefreshVideo()
{
    video_need_refresh = 1;
}

GUI_Texture *Emu_GetVideoTexture()
{
    return video_texture;
}

void Emu_ReshowCtrlPlayer()
{
    show_player_micros = sceKernelGetProcessTimeWide() + SHOW_PLAYER_DURATION_MICROS;
}

void Emu_MakeVideoBaseWH(uint32_t *res_width, uint32_t *res_height)
{
    if (!res_width || !res_height)
        return;

    uint32_t base_width = core_system_av_info.geometry.base_width;
    uint32_t base_height = core_system_av_info.geometry.base_height;

    if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_BY_GAME_RESOLUTION)
    {
        *res_width = base_width;
        *res_height = base_height;
        return;
    }

    float aspect_ratio = 0;

    if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_DEFAULT)
    {
        aspect_ratio = core_system_av_info.geometry.aspect_ratio;
    }
    else if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_BY_DEV_SCREEN)
    {
        aspect_ratio = (float)GUI_SCREEN_WIDTH / (float)GUI_SCREEN_HEIGHT;
    }
    else if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_8_7)
    {
        aspect_ratio = 8.f / 7.f;
    }
    else if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_4_3)
    {
        aspect_ratio = 4.f / 3.f;
    }
    else if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_3_2)
    {
        aspect_ratio = 3.f / 2.f;
    }
    else if (graphics_config.aspect_ratio == TYPE_DISPLAY_RATIO_16_9)
    {
        aspect_ratio = 16.f / 9.f;
    }

    if (aspect_ratio == 0)
        aspect_ratio = (float)base_width / (float)base_height;

    uint32_t new_width = base_width;
    uint32_t new_height = new_width / aspect_ratio;
    if (new_height < base_height)
    {
        new_height = base_height;
        new_width = new_height * aspect_ratio;
    }
    *res_width = new_width;
    *res_height = new_height;
}

void Emu_MakeVideoDisplayWH(uint32_t *res_width, uint32_t *res_height, int rotate)
{
    uint32_t base_width = 0, base_height = 0;
    uint32_t new_width = 0, new_height = 0;
    float aspect_ratio;

    Emu_MakeVideoBaseWH(&base_width, &base_height);

    if (rotate == 1 || rotate == 3)
    {
        uint32_t temp_base_width = base_width;
        base_width = base_height;
        base_height = temp_base_width;
    }

    aspect_ratio = (float)base_width / (float)base_height;

    if (graphics_config.display_size == TYPE_DISPLAY_SIZE_2X)
    { // 2倍大小
        new_width = base_width * 2;
        new_height = base_height * 2;
    }
    else if (graphics_config.display_size == TYPE_DISPLAY_SIZE_3X)
    { // 3倍大小
        new_width = base_width * 3;
        new_height = base_height * 3;
    }
    else if (graphics_config.display_size == TYPE_DISPLAY_SIZE_FULL)
    { // 铺满屏幕
        new_height = GUI_SCREEN_HEIGHT;
        new_width = new_height * aspect_ratio;
    }
    else
    { // 1倍大小
        new_width = base_width;
        new_height = base_height;
    }

    // 检测越界
    if (new_width > GUI_SCREEN_WIDTH)
    {
        new_width = GUI_SCREEN_WIDTH;
        new_height = new_width / aspect_ratio;
    }
    if (new_height > GUI_SCREEN_HEIGHT)
    {
        new_height = GUI_SCREEN_HEIGHT;
        new_width = new_height * aspect_ratio;
    }

    *res_width = new_width;
    *res_height = new_height;
}

uint32_t *Emu_GetVideoScreenshotData(uint32_t *width, uint32_t *height, uint64_t *size, int rotate)
{
    if (!video_texture)
        return NULL;

    GUI_Texture *rendert_tex = NULL;
    uint32_t *conver_data = NULL;
    uint32_t conver_width = *width;
    uint32_t conver_height = *height;
    float aspect_ratio = (float)*width / (float)*height;

    if (conver_width > GUI_SCREEN_WIDTH)
    {
        conver_width = GUI_SCREEN_WIDTH;
        conver_height = conver_width / aspect_ratio;
    }
    if (conver_height > GUI_SCREEN_HEIGHT)
    {
        conver_height = GUI_SCREEN_HEIGHT;
        conver_width = conver_height * aspect_ratio;
    }
    conver_width = (conver_width + 7) & ~7;
    if (conver_width != *width)
        conver_height = conver_width / aspect_ratio;

    float rotate_rad = 0;
    if (rotate == 1)
        rotate_rad = M_PI_2;
    else if (rotate == 2)
        rotate_rad = M_PI_2 * 2;
    else if (rotate == 3)
        rotate_rad = M_PI_2 * 3;

    float x_scale, y_scale;
    if (rotate == 1 || rotate == 3)
    {
        x_scale = (float)conver_height / (float)video_width;
        y_scale = (float)conver_width / (float)video_height;
    }
    else
    {
        x_scale = (float)conver_width / (float)video_width;
        y_scale = (float)conver_height / (float)video_height;
    }

    rendert_tex = GUI_CreatTextureRendertarget(GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT, GUI_TEXTURE_FORMAT_U8U8U8U8_ABGR);
    if (!rendert_tex)
        goto END;

    GUI_SetRendertarget(rendert_tex);
    GUI_StartDrawing();
    if (video_shader)
        GUI_DrawTextureShaderPartScalRotate(video_texture, video_shader, conver_width / 2, conver_height / 2, 0, 0,
                                            video_width, video_height, x_scale, y_scale, rotate_rad);
    else
        GUI_DrawTexturePartScalRotate(video_texture, conver_width / 2, conver_height / 2, 0, 0,
                                      video_width, video_height, x_scale, y_scale, rotate_rad);
    GUI_EndDrawing();
    GUI_SetRendertarget(NULL);
    GUI_WaitRenderDone();

    uint64_t conver_size = conver_width * conver_height * 4;
    conver_data = (uint32_t *)malloc(conver_size);
    if (!conver_data)
        goto END;
    uint8_t *conver_datap = (uint8_t *)conver_data;
    uint32_t conver_stride = conver_width * 4;
    uint8_t *tex_datap = (uint8_t *)GUI_GetTextureDatap(rendert_tex);
    uint32_t tex_stride = GUI_GetTextureStride(rendert_tex);

    int i;
    for (i = 0; i < conver_height; i++)
    {
        memcpy(conver_datap, tex_datap, conver_stride);
        tex_datap += tex_stride;
        conver_datap += conver_stride;
    }

    *width = conver_width;
    *height = conver_height;
    *size = conver_size;

    // AppLog("conver_width: %d, conver_height: %d\n", conver_width, conver_height);

END:
    if (rendert_tex)
        GUI_DestroyTexture(rendert_tex);
    return conver_data;
}

int Emu_SaveVideoScreenshot(char *path)
{
    int ret = 0;

    char parent_path[MAX_PATH_LENGTH];
    MakeBaseDirectory(parent_path, path, MAX_PATH_LENGTH);
    CreateFolder(parent_path);

    uint32_t base_width, base_height;
    Emu_MakeVideoBaseWH(&base_width, &base_height);

    uint32_t screenshot_width, screenshot_height;
    if (graphics_config.display_rotate == 1 || graphics_config.display_rotate == 3)
    {
        screenshot_width = base_height;
        screenshot_height = base_width;
    }
    else
    {
        screenshot_width = base_width;
        screenshot_height = base_height;
    }

    uint64_t screenshot_size = 0;
    uint32_t *screenshot_buf = Emu_GetVideoScreenshotData(&screenshot_width, &screenshot_height, &screenshot_size, graphics_config.display_rotate);
    if (!screenshot_buf)
        return -1;

    ret = WritePngFile(path, (unsigned char *)screenshot_buf, screenshot_width, screenshot_height, 8);
    free(screenshot_buf);
    return ret;
}

static int creatVideoTexture()
{
    if (video_texture) // Free old
    {
        GUI_WaitRenderDone();
        GUI_DestroyTexture(video_texture);
        video_texture = NULL;
    }

    video_texture = GUI_CreatTextureFormat(core_system_av_info.geometry.max_width, core_system_av_info.geometry.max_height, video_texture_format);

    if (!video_texture)
    {
        AppLog("[VIDEO] create video texture failed\n");
        return -1;
    }
    return 0;
}

static int creatOverlayTexture()
{
    if (overlay_texture) // Free old
    {
        GUI_WaitRenderDone();
        GUI_DestroyTexture(overlay_texture);
        overlay_texture = NULL;
    }

    if (graphics_config.overlay_select < 1) // Select none
        return 0;

    OverlayListEntry *entry = OverlayListGetEntryByNumber(&setting_overlay_list, graphics_config.overlay_select - 1);
    if (!entry || !entry->image_name) // Find none
        return -1;

    char path[MAX_PATH_LENGTH];
    if (private_assets_dir) // Try load image from private assets dir
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s/%s", private_assets_dir, OVERLAYS_DIR_NAME, entry->image_name);
        overlay_texture = GUI_LoadPNGFile(path);
    }
    if (!overlay_texture) // Try load image from public assets dir
    {
        snprintf(path, MAX_PATH_LENGTH, "%s/%s/%s", public_assets_dir, OVERLAYS_DIR_NAME, entry->image_name);
        overlay_texture = GUI_LoadPNGFile(path);
    }
    if (!overlay_texture)
    {
        AppLog("[VIDEO] create overlay texture failed\n");
        return -1;
    }
    return 0;
}

static int refreshVideo()
{
    OverlayListEntry *overlay_config = NULL;
    int display_rotate = 0;
    uint32_t display_width = 0, display_height = 0;

    if (!video_okay)
        return -1;

    // Video texture
    if (old_video_texture_format != video_texture_format)
    {
        creatVideoTexture();
        old_video_texture_format = video_texture_format;
    }

    if (!video_texture)
        return -1;

    // Overlay texture
    if (old_video_overlay_select != graphics_config.overlay_select)
    {
        creatOverlayTexture();
        old_video_overlay_select = graphics_config.overlay_select;
    }

    // Overlay config
    if (graphics_config.overlay_select > 0)
        overlay_config = OverlayListGetEntryByNumber(&setting_overlay_list, graphics_config.overlay_select - 1);

    // Rotate
    display_rotate = graphics_config.display_rotate;
    if (overlay_config && overlay_config->viewport_rotate)
        display_rotate = *(overlay_config->viewport_rotate);
    if (display_rotate == 1)
        video_rotate_rad = M_PI_2;
    else if (display_rotate == 2)
        video_rotate_rad = M_PI_2 * 2;
    else if (display_rotate == 3)
        video_rotate_rad = M_PI_2 * 3;
    else
        video_rotate_rad = 0.0f;

    // Width & height
    if (overlay_config && overlay_config->viewport_width && overlay_config->viewport_height)
    {
        display_width = *(overlay_config->viewport_width);
        display_height = *(overlay_config->viewport_height);
    }
    else
    {
        Emu_MakeVideoDisplayWH(&display_width, &display_height, display_rotate);
    }

    // Coord
    video_x = GUI_SCREEN_HALF_WIDTH;
    video_y = GUI_SCREEN_HALF_HEIGHT;
    if (overlay_config)
    {
        if (overlay_config->viewport_x)
            video_x = *(overlay_config->viewport_x) + display_width / 2;
        if (overlay_config->viewport_y)
            video_y = *(overlay_config->viewport_y) + display_height / 2;
    }

    // Scale
    if (display_rotate == 1 || display_rotate == 3)
    {
        video_x_scale = (float)display_height / (float)video_width;
        video_y_scale = (float)display_width / (float)video_height;
    }
    else
    {
        video_x_scale = (float)display_width / (float)video_width;
        video_y_scale = (float)display_height / (float)video_height;
    }

    // Shader
    if (graphics_config.graphics_filtering == 1)
        video_shader = lcd3x_shader;
    else if (graphics_config.graphics_filtering == 2)
        video_shader = sharp_bilinear_simple_shader;
    else if (graphics_config.graphics_filtering == 3)
        video_shader = sharp_bilinear_shader;
    else if (graphics_config.graphics_filtering == 4)
        video_shader = advanced_aa_shader;
    else
        video_shader = NULL;

    // Filters
    if (graphics_config.graphics_smooth)
        GUI_SetTextureFilter(video_texture, GUI_TEXTURE_FILTER_LINEAR, GUI_TEXTURE_FILTER_LINEAR);
    else
        GUI_SetTextureFilter(video_texture, GUI_TEXTURE_FILTER_POINT, GUI_TEXTURE_FILTER_POINT);

    return 0;
}

void Emu_DrawVideo()
{
    if (!video_okay)
        return;

    if (video_need_refresh)
    {
        refreshVideo();
        video_need_refresh = 0;
    }

    if (!video_texture)
        return;

    if (overlay_texture && graphics_config.overlay_mode == 1)
        GUI_DrawTexture(overlay_texture, 0.0f, 0.0f);

    if (video_shader)
        GUI_DrawTextureShaderPartScalRotate(video_texture, video_shader, video_x, video_y, 0, 0, video_width, video_height, video_x_scale, video_y_scale, video_rotate_rad);
    else
        GUI_DrawTexturePartScalRotate(video_texture, video_x, video_y, 0, 0, video_width, video_height, video_x_scale, video_y_scale, video_rotate_rad);

    if (overlay_texture && graphics_config.overlay_mode == 0)
        GUI_DrawTexture(overlay_texture, 0.0f, 0.0f);
}

int Emu_InitVideo()
{
    AppLog("[VIDEO] video init...\n");

    video_okay = 0;
    video_pause = 1;

    if (creatVideoTexture() < 0)
        return -1;
    creatOverlayTexture();

    video_width = core_system_av_info.geometry.base_width;
    video_height = core_system_av_info.geometry.base_height;
    old_video_texture_format = video_texture_format;
    old_video_overlay_select = graphics_config.overlay_select;
    video_need_refresh = 1;
    video_okay = 1;

    if (control_config.map_port != 0)
        Emu_ReshowCtrlPlayer();

    AppLog("[VIDEO] max width: %d, max height: %d\n", core_system_av_info.geometry.max_width, core_system_av_info.geometry.max_height);
    AppLog("[VIDEO] base width: %d, base height: %d\n", core_system_av_info.geometry.base_width, core_system_av_info.geometry.base_height);
    AppLog("[VIDEO] fps: %.2f\n", core_system_av_info.timing.fps);
    AppLog("[VIDEO] video init done\n");

    return 0;
}

int Emu_DeinitVideo()
{
    AppLog("[VIDEO] video deinit...\n");

    video_okay = 0;
    video_pause = 1;

    GUI_WaitRenderDone();
    if (video_texture)
    {
        GUI_DestroyTexture(video_texture);
        video_texture = NULL;
    }
    if (overlay_texture)
    {
        GUI_DestroyTexture(overlay_texture);
        overlay_texture = NULL;
    }

    GUI_SetVblankWait(1);

    AppLog("[VIDEO] video deinit done\n");
    return 0;
}

static void refreshFps()
{
    uint64_t cur_micros = sceKernelGetProcessTimeWide();
    uint64_t interval_micros = cur_micros - last_fps_micros;
    if (interval_micros > 1000000)
    {
        last_fps_micros = cur_micros;
        video_fps = (video_frames / (double)interval_micros) * 1000000.0f;
        video_frames = 0;
    }
    video_frames++;
}
/*
static void drawHeadMemInfo()
{
    char total_string[16];
    char free_string[16];
    GetSizeString(total_string, GetTotalHeadMemSize());
    GetSizeString(free_string, GetFreeHeadMemSize());

    GUI_DrawTextf( 0, GUI_GetFontSize(), COLOR_WHITE, "%s/%s", free_string, total_string);
}
*/
static void displayVideo()
{
    GUI_StartDrawing();

    Emu_DrawVideo();

    if (graphics_config.show_fps)
    {
        refreshFps();
        GUI_DrawTextf(0.0f, 0.0f, COLOR_WHITE, "FPS: %.2f", video_fps);
    }

    if (Emu_GetCurrentRunSpeed() != 1.0f)
    {
        char fps_scale_string[12];
        snprintf(fps_scale_string, 12, "%.2fX", Emu_GetCurrentRunSpeed());
        GUI_DrawText(GUI_SCREEN_WIDTH - GUI_GetTextWidth(fps_scale_string), 0.0f, COLOR_WHITE, fps_scale_string);
    }

    if (show_player_micros > 0)
    {
        uint64_t cur_micros = sceKernelGetProcessTimeWide();
        if (cur_micros < show_player_micros)
            GUI_DrawTextf(0.0f, GUI_SCREEN_HEIGHT - GUI_GetFontSize(), COLOR_WHITE, "%s: %dP", cur_lang[CTRL_PLAYER], control_config.map_port + 1);
        else
            show_player_micros = 0;
    }

    // drawHeadMemInfo();

    GUI_EndDrawing();
}

void Retro_VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    if (!video_okay || video_pause)
        return;

    if (data && pitch > 0)
    {
        if ((video_width != width) || (video_height != height))
        {
            video_width = width;
            video_height = height;
            video_need_refresh = 1;
        }

        const uint8_t *in_data = (const uint8_t *)data;
        uint8_t *out_data = (uint8_t *)GUI_GetTextureDatap(video_texture);
        unsigned int in_pitch = pitch;
        unsigned int out_pitch = GUI_GetTextureStride(video_texture);

        int i;
        for (i = 0; i < height; i++)
        {
            memcpy(out_data, in_data, in_pitch);
            in_data += in_pitch;
            out_data += out_pitch;
        }
    }

    displayVideo();
}
