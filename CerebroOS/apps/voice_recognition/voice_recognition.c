#include "voice_recognition.h"
#include "../ai_models/tensorflow_lite/tensorflow_lite.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LABELS 10  // Adjust to the number of commands you have
#define AUDIO_INPUT_SIZE 16000 // Example size, adjust based on your model
#define LABEL_BUFFER_SIZE 50  // Max length of a command label

static const char* labels[MAX_LABELS] = {
    "turn on light",
    "turn off light",
    "play music",
    "play next music",
    "play previews music",
    "pause music",
    // Add your voice command labels here
};

// Error Codes
typedef enum {
    VOICE_RECOGNITION_ERROR_NONE,
    VOICE_RECOGNITION_ERROR_TFLITE_INIT,
    VOICE_RECOGNITION_ERROR_INFERENCE,
    VOICE_RECOGNITION_ERROR_AUDIO_CAPTURE, // Add if using real audio input
} VoiceRecognitionError;

// Function to initialize Voice Recognition
VoiceRecognitionError init_voice_recognition(const char* modelPath) {
    TfLiteError tflite_error = setup_tflite(modelPath); // Pass model path
    if (tflite_error != kTfLiteOk) {
        return VOICE_RECOGNITION_ERROR_TFLITE_INIT;
    }
    return VOICE_RECOGNITION_ERROR_NONE;
}

// Function to process audio samples
VoiceRecognitionError process_audio_sample(const int16_t* audio_data, int audio_size, char* recognizedCommand) {
    if (!audio_data || audio_size <= 0) {
        return VOICE_RECOGNITION_ERROR_AUDIO_CAPTURE; // Or a more specific error
    }

    // Preprocessing (if needed)
    float input_data[AUDIO_INPUT_SIZE];
    for (int i = 0; i < audio_size; i++) {
        input_data[i] = (float)audio_data[i] / 32768.0f; // Normalize to [-1, 1]
    }

    // Run Inference
    float output[MAX_LABELS];
    TfLiteError tflite_error = run_inference(input_data, AUDIO_INPUT_SIZE, output, MAX_LABELS);
    if (tflite_error != kTfLiteOk) {
        return VOICE_RECOGNITION_ERROR_INFERENCE;
    }

    // Find Command with Highest Probability
    int max_index = 0;
    float max_probability = output[0];
    for (int i = 1; i < MAX_LABELS; i++) {
        if (output[i] > max_probability) {
            max_index = i;
            max_probability = output[i];
        }
    }

    // Threshold for Recognition Confidence (optional)
    if (max_probability > 0.8f) { // Adjust the threshold as needed
        strncpy(recognizedCommand, labels[max_index], LABEL_BUFFER_SIZE);
        return VOICE_RECOGNITION_ERROR_NONE;
    } else {
        strcpy(recognizedCommand, "unknown"); // or set recognizedCommand to an empty string
        return VOICE_RECOGNITION_ERROR_NONE; // No error, but not confident enough
    }
}
