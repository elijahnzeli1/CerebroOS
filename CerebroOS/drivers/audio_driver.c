#include "audio_driver.h"
#include <SDL2/SDL.h>
#include <math.h> 

#define SAMPLE_RATE 22050          // Reduced sample rate for smaller devices
#define AUDIO_BUFFER_SIZE 1024     // Smaller buffer for reduced latency

static SDL_AudioDeviceID audio_device = 0;
static float current_volume = 1.0f;
static bool audio_playing = false;  // Flag to track audio playback

// Audio Callback (optimized for performance)
void audio_callback(void* userdata, Uint8* stream, int len) {
    static float t = 0;
    int16_t* samples = (int16_t*)stream;
    int sample_count = len / sizeof(int16_t);

    // Use a pre-calculated table of sine values for speed
    static int16_t sine_table[AUDIO_BUFFER_SIZE];
    static bool sine_table_initialized = false;

    // Initialize sine table only once
    if (!sine_table_initialized) {
        for (int i = 0; i < AUDIO_BUFFER_SIZE; ++i) {
            sine_table[i] = (int16_t)(sin(2.0 * M_PI * i * 440.0 / SAMPLE_RATE) * 32767.0f);
        }
        sine_table_initialized = true;
    }

    // Fill the stream using the sine table
    for (int i = 0; i < sample_count; ++i) {
        samples[i] = (int16_t)(sine_table[i % AUDIO_BUFFER_SIZE] * current_volume);
        t += 2.0 * M_PI * 440.0 / SAMPLE_RATE; 
        if (t > 2.0 * M_PI) t -= 2.0 * M_PI;
    }
}

// Audio Initialization (with error handling)
AudioError audio_init() {
    // SDL Audio Initialization
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return AUDIO_ERROR_INIT;
    }

    SDL_AudioSpec desired, obtained;
    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq = SAMPLE_RATE;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1;  
    desired.samples = AUDIO_BUFFER_SIZE;
    desired.callback = audio_callback;

    // Open Audio Device (with error handling)
    audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (audio_device == 0) {
        SDL_Log("Failed to open audio device: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return AUDIO_ERROR_OPEN;
    }

    return AUDIO_ERROR_NONE; 
}


// Audio Playback
AudioError audio_play(const int16_t* samples, size_t sample_count) {
    if (audio_device == 0) { //check if the audio device is not initialized
        SDL_Log("Audio Device is not initialized");
        return AUDIO_ERROR_NOT_INITIALIZED;
    }
    if (audio_playing) {
        return AUDIO_ERROR_ALREADY_PLAYING;
    }

    int queued = SDL_QueueAudio(audio_device, samples, sample_count * sizeof(int16_t));
    if (queued < 0) {
        SDL_Log("Failed to queue audio: %s", SDL_GetError());
        return AUDIO_ERROR_PLAYBACK;
    }

    SDL_PauseAudioDevice(audio_device, 0);
    audio_playing = true; //set the audio playing flag
    return AUDIO_ERROR_NONE;
}
// Audio Recording (Placeholder - Implementation would be hardware-specific)

// Audio Set Volume
AudioError audio_set_volume(float volume) {
    if (volume < 0.0f || volume > 1.0f) {
        return AUDIO_ERROR_INVALID_VOLUME; //volume is not within the correct range
    }
    current_volume = volume;
    return AUDIO_ERROR_NONE;
}
// Audio Stop function
AudioError audio_stop() {
    if (!audio_playing) { //check if the audio playing flag is false
        return AUDIO_ERROR_NOT_PLAYING;
    }
    SDL_PauseAudioDevice(audio_device, 1);
    audio_playing = false;
    return AUDIO_ERROR_NONE;

}

// Audio Cleanup (with error checking)
void audio_cleanup() {
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
