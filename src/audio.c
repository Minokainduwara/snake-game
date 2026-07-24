#include "audio.h"
#include <math.h>
#include <stdlib.h>

static Sound eat_sound;
static Sound death_sound;
static Sound start_sound;

// Generate a simple tone as a Wave
static Wave GenerateTone(float frequency, float duration, float volume) {
    int sampleRate = 44100;
    int sampleCount = (int)(sampleRate * duration);
    unsigned char *data = (unsigned char *)RL_MALLOC(sampleCount * sizeof(short));

    short *samples = (short *)data;
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        // Sine wave with a slight envelope (fade in/out)
        float envelope = 1.0f;
        float fadeLen = 0.01f; // 10ms fade
        if (t < fadeLen) envelope = t / fadeLen;
        if (t > duration - fadeLen) envelope = (duration - t) / fadeLen;

        float sample = sinf(2.0f * PI * frequency * t) * envelope * volume;
        samples[i] = (short)(sample * 32767.0f);
    }

    Wave wave = { 0 };
    wave.data = data;
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;

    return wave;
}

void Audio_Init(void) {
    InitAudioDevice();

    // Eat sound: short rising beep
    Wave eat_wave = GenerateTone(440.0f, 0.1f, 0.5f);
    eat_sound = LoadSoundFromWave(eat_wave);
    UnloadWave(eat_wave);

    // Death sound: low descending tone
    {
        int sampleRate = 44100;
        float duration = 0.4f;
        int sampleCount = (int)(sampleRate * duration);
        unsigned char *data = (unsigned char *)RL_MALLOC(sampleCount * sizeof(short));
        short *samples = (short *)data;

        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float freq = 300.0f - (200.0f * t / duration); // descending
            float envelope = 1.0f;
            float fadeLen = 0.02f;
            if (t < fadeLen) envelope = t / fadeLen;
            if (t > duration - fadeLen) envelope = (duration - t) / fadeLen;
            float sample = sinf(2.0f * PI * freq * t) * envelope * 0.5f;
            samples[i] = (short)(sample * 32767.0f);
        }

        Wave death_wave = { 0 };
        death_wave.data = data;
        death_wave.frameCount = sampleCount;
        death_wave.sampleRate = sampleRate;
        death_wave.sampleSize = 16;
        death_wave.channels = 1;

        death_sound = LoadSoundFromWave(death_wave);
        UnloadWave(death_wave);
    }

    // Start sound: two ascending beeps
    {
        int sampleRate = 44100;
        float duration = 0.3f;
        int sampleCount = (int)(sampleRate * duration);
        unsigned char *data = (unsigned char *)RL_MALLOC(sampleCount * sizeof(short));
        short *samples = (short *)data;

        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float freq = (t < duration / 2) ? 330.0f : 660.0f;
            float envelope = 1.0f;
            float fadeLen = 0.01f;
            if (t < fadeLen) envelope = t / fadeLen;
            if (t > duration - fadeLen) envelope = (duration - t) / fadeLen;
            float sample = sinf(2.0f * PI * freq * t) * envelope * 0.5f;
            samples[i] = (short)(sample * 32767.0f);
        }

        Wave start_wave = { 0 };
        start_wave.data = data;
        start_wave.frameCount = sampleCount;
        start_wave.sampleRate = sampleRate;
        start_wave.sampleSize = 16;
        start_wave.channels = 1;

        start_sound = LoadSoundFromWave(start_wave);
        UnloadWave(start_wave);
    }
}

void Audio_PlayEat(void) {
    PlaySound(eat_sound);
}

void Audio_PlayDeath(void) {
    PlaySound(death_sound);
}

void Audio_PlayStart(void) {
    PlaySound(start_sound);
}

void Audio_Unload(void) {
    UnloadSound(eat_sound);
    UnloadSound(death_sound);
    UnloadSound(start_sound);
    CloseAudioDevice();
}