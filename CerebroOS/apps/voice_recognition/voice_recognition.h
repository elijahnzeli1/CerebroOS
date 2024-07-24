#ifndef VOICE_RECOGNITION_H
#define VOICE_RECOGNITION_H

#include <stdint.h>   // For standard integer types
#include <stddef.h>   // For size_t

// Error Codes for Voice Recognition Operations
typedef enum {
    VOICE_RECOGNITION_ERROR_NONE = 0,         // No error
    VOICE_RECOGNITION_ERROR_TFLITE_INIT,      // TensorFlow Lite initialization error
    VOICE_RECOGNITION_ERROR_INFERENCE,        // Model inference error
    VOICE_RECOGNITION_ERROR_AUDIO_CAPTURE,    // Audio capture error
    VOICE_RECOGNITION_ERROR_MODEL_LOAD,       // Error loading the model
    // ... Add more error codes as needed
} VoiceRecognitionError;

// Function Declarations

// Initialize the voice recognition module
// Parameters:
//   - modelPath: Path to the TensorFlow Lite model file
VoiceRecognitionError init_voice_recognition(const char* modelPath);

// Process a buffer of audio samples
// Parameters:
//   - audio_data: Pointer to the audio data buffer (int16_t samples)
//   - audio_size: Number of samples in the buffer
//   - recognizedCommand: Output buffer to store the recognized command (must be large enough)
// Returns:
//   - A VoiceRecognitionError code indicating success or failure
VoiceRecognitionError process_audio_sample(const int16_t* audio_data, int audio_size, char* recognizedCommand);

// Cleanup the voice recognition module (release resources)
void cleanup_voice_recognition();

// Optional Functions (Consider adding these for more flexibility)

// Start continuous voice recognition (listens for commands in the background)
VoiceRecognitionError start_continuous_recognition();

// Stop continuous voice recognition
VoiceRecognitionError stop_continuous_recognition();

// Get the current recognition status (e.g., active, inactive, error)
VoiceRecognitionError get_recognition_status();

#endif // VOICE_RECOGNITION_H
