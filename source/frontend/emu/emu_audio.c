#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/audioout.h>
#include <speex/speex_resampler.h>

#include "emu/emu.h"
#include "utils.h"

#define AUDIO_MAX_VOLUME 0x8000
#define AUDIO_DEFAULT_SAMPLE_RATE 48000u
#define AUDIO_OUTPUT_COUNT 512u

typedef struct
{
    int16_t *data[2];
    size_t data_len;
    int n_samples;
} AudioSoundOut;

typedef struct
{
    int16_t *data;
    size_t data_len;
    size_t write_pos;
    size_t read_pos;
} AudioSoundBuffer;

typedef struct
{
    SpeexResamplerState *resampler_state;
    int quality;
    uint32_t in_rate;
    uint32_t out_rate;
    const int16_t *in_data;
    uint32_t in_data_len;
    int16_t *out_data;
    uint32_t out_data_len;
} AudioResampler;

typedef struct
{
    int okay;
    int pause;
    int stop;
    int stereo;
    uint32_t sample_rate;
    int left_volume;
    int right_volume;
    int thread_handle;
    int output_handle;
    int need_resample;
    AudioResampler resampler;
    AudioSoundOut sound_out;
    AudioSoundBuffer sound_buffer;
    void *userdata;
} AudioState;

static AudioState game_audio_state = {0};

static void AudioResamplerDestroy(AudioResampler *resampler)
{
    AppLog("[AUDIO] Resampler destroy...\n");

    if (resampler)
    {
        if (resampler->resampler_state)
            speex_resampler_destroy(resampler->resampler_state);
        resampler->resampler_state = NULL;

        if (resampler->out_data)
            free(resampler->out_data);
        resampler->out_data = NULL;

        memset(resampler, 0, sizeof(AudioResampler));
    }

    AppLog("[AUDIO] Resampler destroy OK!\n");
}

static int AudioResamplerInit(AudioResampler *resampler)
{
    AppLog("[AUDIO] Audio resampler init...\n");

    if (!resampler)
        goto FAILED;

    if (resampler->resampler_state)
    {
        speex_resampler_set_rate(resampler->resampler_state, resampler->in_rate, resampler->out_rate);
        speex_resampler_set_quality(resampler->resampler_state, resampler->quality);
    }
    else
    {
        resampler->resampler_state = speex_resampler_init(1, resampler->in_rate, resampler->out_rate, resampler->quality, NULL);
    }
    if (!resampler->resampler_state)
        goto FAILED;
    // speex_resampler_skip_zeros(resampler->resampler_state);

    AppLog("[AUDIO] Audio resampler init OK!\n");
    return 0;

FAILED:
    AppLog("[AUDIO] Audio resampler init failed!\n");
    Emu_DeinitAudio();
    sceKernelExitProcess(0);
    return -1;
}

static int AudioResamplerProcessInt(AudioResampler *resampler)
{
    return speex_resampler_process_int(resampler->resampler_state, 0, resampler->in_data, &(resampler->in_data_len), resampler->out_data, &(resampler->out_data_len));
}

void Emu_CleanAudioSound()
{
    if (!game_audio_state.okay)
        return;

    AudioSoundOut *sound_out = &(game_audio_state.sound_out);
    AudioSoundBuffer *sound_buffer = &(game_audio_state.sound_buffer);

    int i;
    for (i = 0; i < 2; i++)
    {
        if (sound_out->data[i])
            memset(sound_out->data[i], 0, sound_out->data_len * sizeof(int16_t));
    }

    if (sound_buffer->data)
        memset(sound_buffer->data, 0, sound_buffer->data_len * sizeof(int16_t));
    sound_buffer->write_pos = 0;
    sound_buffer->read_pos = 0;
}

void Emu_PauseAudio()
{
    game_audio_state.pause = 1;
}

void Emu_ResumeAudio()
{
    game_audio_state.pause = 0;
}

int Emu_SetAudioVolume(int left, int right)
{
    if (!game_audio_state.okay)
        return -1;

    if (left > AUDIO_MAX_VOLUME)
        left = AUDIO_MAX_VOLUME;
    if (right > AUDIO_MAX_VOLUME)
        right = AUDIO_MAX_VOLUME;
    game_audio_state.left_volume = left;
    game_audio_state.right_volume = right;

    int vols[2] = {left, right};
    return sceAudioOutSetVolume(game_audio_state.output_handle, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vols);
}

int Emu_SetAudioConfig(int samples, uint16_t freq, int stereo)
{
    if (!game_audio_state.okay)
        return -1;

    game_audio_state.sound_out.n_samples = samples;
    game_audio_state.sample_rate = freq;
    game_audio_state.stereo = stereo;

    return sceAudioOutSetConfig(game_audio_state.output_handle, samples, freq, stereo);
}

static int AudioOutput(void *buf)
{
    if (!game_audio_state.okay)
        return -1;

    return sceAudioOutOutput(game_audio_state.output_handle, buf);
}

static void AudioSetSampleRate(uint16_t sample_rate)
{
    uint32_t sample_rates[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
    int n_sample_rates = sizeof(sample_rates) / sizeof(uint32_t);
    int neglect = 50;

    int i;
    for (i = 0; i < n_sample_rates; i++)
    {
        if ((sample_rate >= sample_rates[i] - neglect) && (sample_rate <= sample_rates[i] + neglect))
            break;
    }

    if (i >= n_sample_rates)
    {
        game_audio_state.need_resample = 1;
        game_audio_state.sample_rate = AUDIO_DEFAULT_SAMPLE_RATE;

        AudioResampler *resampler = &(game_audio_state.resampler);
        resampler->quality = 1;
        resampler->in_rate = (uint32_t)core_system_av_info.timing.sample_rate;
        resampler->out_rate = game_audio_state.sample_rate;
        AppLog("[AUDIO] Audio need resample %u to %u\n", resampler->in_rate, resampler->out_rate);
        AudioResamplerInit(resampler);
    }
    else
    {
        game_audio_state.need_resample = 0;
        game_audio_state.sample_rate = sample_rates[i];
        AppLog("[AUDIO] Audio sample rate: %d\n", game_audio_state.sample_rate);
    }
}

int Emu_UpdateAudioSampleRate(uint32_t sample_rate)
{
    int ret = 0;

    AppLog("[AUDIO] Update audio frequency...\n");

    if (!game_audio_state.okay || sample_rate == 0)
    {
        AppLog("[AUDIO] Update audio frequency failed!\n");
        return -1;
    }

    int audio_pause = game_audio_state.pause;
    if (!audio_pause)
        Emu_PauseAudio();

    AudioSetSampleRate(sample_rate);
    ret = Emu_SetAudioConfig(game_audio_state.sound_out.n_samples, game_audio_state.sample_rate, game_audio_state.stereo);
    Emu_CleanAudioSound();

    if (!audio_pause)
        Emu_ResumeAudio();

    AppLog("[AUDIO] Update audio frequency OK!\n");

    return ret;
}

static int AudioThreadFunc(unsigned int args, void *argp)
{
    AppLog("[AUDIO] Audio thread start\n");

    AudioSoundOut *sound_out = &(game_audio_state.sound_out);
    AudioSoundBuffer *sound_buffer = &(game_audio_state.sound_buffer);
    int out_data_idx = 0;

    while (!game_audio_state.stop)
    {
        if (game_audio_state.pause)
        {
            sceKernelDelayThread(1000);
            continue;
        }

        int read_pos = sound_buffer->read_pos;
        int write_pos = sound_buffer->write_pos;

        int16_t *in_data = sound_buffer->data;
        uint32_t in_data_len = sound_buffer->data_len;

        int16_t *out_data = sound_out->data[out_data_idx];
        uint32_t out_data_len = sound_out->data_len;

        // memset(out_data, 0, out_data_len * sizeof(int16_t));

        int i;
        for (i = 0; i < out_data_len; i++)
        {
            if (read_pos == write_pos)
            {
                int j;
                for (j = i; j < out_data_len; j++)
                    out_data[j] = 0;
                break;
            }

            out_data[i] = in_data[read_pos];
            in_data[read_pos] = 0;
            read_pos++;
            if (read_pos >= in_data_len)
                read_pos = 0;
        }
        sound_buffer->read_pos = read_pos;

        AudioOutput(out_data);
        out_data_idx = (out_data_idx ? 0 : 1);
    }

    AppLog("[AUDIO] Audio thread exit!\n");
    sceKernelExitThread(0);
    return 0;
}

static void AudioOutputShutdown()
{
    AudioSoundOut *sound_out = &(game_audio_state.sound_out);
    AudioSoundBuffer *sound_buffer = &(game_audio_state.sound_buffer);

    AppLog("[AUDIO] Audio output shutdown...\n");

    // Deinit audio out_data port
    if (game_audio_state.output_handle != -1)
    {
        sceAudioOutReleasePort(game_audio_state.output_handle);
        game_audio_state.output_handle = -1;
    }

    AppLog("[AUDIO] Deinit sound out data...\n");

    // Deinit sound out data
    int i;
    for (i = 0; i < 2; i++)
    {
        if (sound_out->data[i])
        {
            free(sound_out->data[i]);
            sound_out->data[i] = NULL;
        }
    }
    sound_out->data_len = 0;
    sound_out->n_samples = 0;

    AppLog("[AUDIO] Deinit sound buffer data...\n");

    // Deinit sound buffer data
    if (sound_buffer->data)
    {
        free(sound_buffer->data);
        sound_buffer->data = NULL;
    }
    sound_buffer->data_len = 0;
    sound_buffer->write_pos = 0;
    sound_buffer->read_pos = 0;

    AppLog("[AUDIO] Audio output shutdown OK!\n");
}

static int AudioOutputInit()
{
    size_t soud_out_size;
    size_t soud_buffer_size;

    AudioSoundOut *sound_out = &(game_audio_state.sound_out);
    AudioSoundBuffer *sound_buffer = &(game_audio_state.sound_buffer);

    AppLog("[AUDIO] Audio output init...\n");

    sound_out->n_samples = AUDIO_OUTPUT_COUNT;

    // Init audio out_data port
    game_audio_state.output_handle = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_VOICE, sound_out->n_samples,
                                                         game_audio_state.sample_rate, game_audio_state.stereo);
    if (game_audio_state.output_handle < 0)
    {
        game_audio_state.output_handle = -1;
        goto FAILED;
    }

    // Init sound out_data data
    if (game_audio_state.stereo)
        sound_out->data_len = sound_out->n_samples * 2;
    else
        sound_out->data_len = sound_out->n_samples;
    soud_out_size = sound_out->data_len * sizeof(int16_t);

    int i;
    for (i = 0; i < 2; i++)
    {
        sound_out->data[i] = (int16_t *)malloc(soud_out_size);
        if (!sound_out->data[i])
            goto FAILED;
        memset(sound_out->data[i], 0, soud_out_size);
    }

    // Init sound buffer data
    sound_buffer->data_len = sound_out->data_len * 10;
    soud_buffer_size = sound_buffer->data_len * sizeof(int16_t);
    sound_buffer->data = (int16_t *)malloc(soud_buffer_size);
    if (!sound_buffer->data)
        goto FAILED;
    memset(sound_buffer->data, 0, soud_buffer_size);
    sound_buffer->read_pos = 0;
    sound_buffer->write_pos = 0;

    AppLog("[AUDIO] Audio output init OK!\n");
    return 0;

FAILED:
    AppLog("[AUDIO] Audio output init failed!\n");
    AudioOutputShutdown();
    return -1;
}

static void AudioThreadShutdown()
{
    AppLog("[AUDIO] Audio thread shutdown...\n");

    game_audio_state.pause = 1;
    game_audio_state.stop = 1;

    // Deinit audio thread
    if (game_audio_state.thread_handle != -1)
    {
        sceKernelWaitThreadEnd(game_audio_state.thread_handle, NULL, NULL);
        sceKernelDeleteThread(game_audio_state.thread_handle);
        game_audio_state.thread_handle = -1;
    }

    AppLog("[AUDIO] Audio thread shutdown OK!\n");
}

static int AudioThreadInit()
{
    AppLog("[AUDIO] Audio thread init...\n");

    game_audio_state.pause = 1;
    game_audio_state.stop = 0;

    // Init audio thread
    game_audio_state.thread_handle = sceKernelCreateThread("emu_audio_thread", AudioThreadFunc,
                                                           0x10000100, 0x10000, 0, 0, NULL);

    if (game_audio_state.thread_handle < 0)
    {
        game_audio_state.thread_handle = -1;
        goto FAILED;
    }

    if (sceKernelStartThread(game_audio_state.thread_handle, 0, NULL) != 0)
        goto FAILED;

    AppLog("[AUDIO] Audio thread init OK!\n");
    return 0;

FAILED:
    AppLog("[AUDIO] Audio thread init failed!\n");
    AudioThreadShutdown();
    return -1;
}

int Emu_InitAudio()
{
    AppLog("[AUDIO] Audio init...\n");

    if (game_audio_state.okay)
        Emu_DeinitAudio();

    game_audio_state.okay = 0;
    game_audio_state.pause = 1;
    game_audio_state.stop = 0;
    game_audio_state.stereo = 1;
    game_audio_state.output_handle = -1;
    game_audio_state.thread_handle = -1;
    game_audio_state.left_volume = AUDIO_MAX_VOLUME;
    game_audio_state.right_volume = AUDIO_MAX_VOLUME;
    game_audio_state.userdata = NULL;

    AudioSetSampleRate(core_system_av_info.timing.sample_rate);

    if (AudioOutputInit() < 0)
        goto FAILED;

    if (AudioThreadInit() < 0)
        goto FAILED;

    game_audio_state.okay = 1;

    AppLog("[AUDIO] Audio init OK!\n");
    return 0;

FAILED:
    AppLog("[AUDIO] Audio init failed\n");
    Emu_DeinitAudio();
    sceKernelExitProcess(0);
    return -1;
}

int Emu_DeinitAudio()
{
    AppLog("[AUDIO] Audio deinit...\n");

    game_audio_state.okay = 0;
    game_audio_state.pause = 1;
    game_audio_state.stop = 1;

    AudioThreadShutdown(); // Thread shutdown must do befor output shutdown!
    AudioOutputShutdown();
    AudioResamplerDestroy(&game_audio_state.resampler);

    AppLog("[AUDIO] Audio deinit OK!\n");

    return 0;
}

size_t Retro_AudioSampleBatchCallback(const int16_t *data, size_t frames)
{
    if (!game_audio_state.okay)
        return frames;

    AudioResampler *resampler = &(game_audio_state.resampler);
    AudioSoundBuffer *sound_buffer = &(game_audio_state.sound_buffer);

    const int16_t *in_data = data;
    uint32_t in_data_len = frames * 2;
    int16_t *out_data = sound_buffer->data;
    uint32_t out_data_len = sound_buffer->data_len;
    int write_pos = sound_buffer->write_pos;

    if (game_audio_state.need_resample && game_audio_state.resampler.resampler_state)
    {
        resampler->in_data = in_data;
        resampler->in_data_len = in_data_len;
        resampler->out_data_len = in_data_len * (double)game_audio_state.sample_rate / core_system_av_info.timing.sample_rate;
        resampler->out_data = (int16_t *)malloc(resampler->out_data_len * sizeof(int16_t));
        if (resampler->out_data)
        {
            AudioResamplerProcessInt(resampler);
            in_data = resampler->out_data;
            in_data_len = resampler->out_data_len;
        }
    }

    int i;
    for (i = 0; i < in_data_len; i++)
    {
        out_data[write_pos] = in_data[i];
        write_pos++;
        if (write_pos >= out_data_len)
            write_pos = 0;
    }
    sound_buffer->write_pos = write_pos;

    if (resampler->out_data)
    {
        free(resampler->out_data);
        resampler->out_data = NULL;
    }

    return frames;
}
