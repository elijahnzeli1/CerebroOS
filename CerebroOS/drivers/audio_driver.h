#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Error Codes for Audio Operations
typedef enum {
    AUDIO_ERROR_NONE = 0,         // No error
    AUDIO_ERROR_INIT,            // Audio initialization error
    AUDIO_ERROR_OPEN,             // Audio device opening error
    AUDIO_ERROR_PLAYBACK,         // Audio playback error
    AUDIO_ERROR_RECORDING,        // Audio recording error
    AUDIO_ERROR_INVALID_VOLUME,   // Invalid volume value
    AUDIO_ERROR_ALREADY_PLAYING,  // Audio already playing
    AUDIO_ERROR_NOT_PLAYING,      // Audio not playing
    AUDIO_ERROR_NOT_INITIALIZED,  // Audio device not initialized
} AudioError;

// Audio Configuration Structure
typedef struct AudioConfig {
    int sampleRate;            // Audio sample rate (e.g., 44100 Hz)
    int channels;              // Number of channels (1 for mono, 2 for stereo)
    int bufferSize;            // Audio buffer size in samples
    // ... additional parameters (e.g., format, bit depth)
} AudioConfig;

// Function Declarations

// Initialize the audio driver
AudioError audio_init(const AudioConfig* config);

// Start or resume audio playback
AudioError audio_play(const int16_t* samples, size_t sample_count);

// Stop audio playback
AudioError audio_stop();

// Record audio from input device (placeholder - implementation is hardware-specific)
AudioError audio_record(int16_t* buffer, size_t sample_count);

// Set audio playback volume
AudioError audio_set_volume(float volume);

// Get current audio volume 
float audio_get_volume();

// Mute audio
AudioError audio_mute();

// Unmute audio
AudioError audio_unmute();

// Cleanup the audio driver (release resources)
void audio_cleanup();

#endif // AUDIO_DRIVER_H
