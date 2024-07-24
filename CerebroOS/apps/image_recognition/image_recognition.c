#include "image_recognition.h"
#include "../ai_models/tensorflow_lite/tensorflow_lite.h"
#include <string.h>  // For memcpy

#define MODEL_INPUT_WIDTH 224
#define MODEL_INPUT_HEIGHT 224
#define MODEL_INPUT_CHANNELS 3

// Image Recognition Initialization
ImageRecognitionError image_recognition_init() {
    // Initialize TensorFlow Lite (with error checking)
    TfLiteError tflite_error = setup_tflite();
    if (tflite_error != kTfLiteOk) {
        return IMAGE_RECOGNITION_ERROR_TFLITE_INIT;
    }
    return IMAGE_RECOGNITION_ERROR_NONE;
}

// Image Preprocessing
static void preprocess_image(const uint8_t* image_data, int width, int height, float* input_data) {
    // Resize and normalize the image to match the model's input requirements
    // You'll need to implement a resizing algorithm here (e.g., bilinear interpolation)
    // For this example, we'll assume the input image is already 224x224
    
    // Normalization 
    for (int i = 0; i < MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * MODEL_INPUT_CHANNELS; i++) {
        input_data[i] = image_data[i] / 255.0f; // Normalize to [0, 1]
    }
}

// Image Classification
const char* image_recognition_classify(const uint8_t* image_data, int width, int height) {
    // Input and Output Data
    static float input_data[MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * MODEL_INPUT_CHANNELS];
    static float output_data[1000]; // Assuming 1000 classes

    // Preprocess Image
    preprocess_image(image_data, width, height, input_data);

    // Run Inference
    TfLiteError tflite_error = run_inference(input_data, 
                                            MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * MODEL_INPUT_CHANNELS, 
                                            output_data, 1000);

    if (tflite_error != kTfLiteOk) {
        // Handle inference error (e.g., log an error message)
        return "Error";
    }

    // Find Class with Highest Probability
    int max_class = 0;
    float max_prob = output_data[0];
    for (int i = 1; i < 1000; i++) {
        if (output_data[i] > max_prob) {
            max_class = i;
            max_prob = output_data[i];
        }
    }
    
    // Return the class label (replace with your actual label mapping)
    static char result[100];  // Larger buffer for label
    snprintf(result, sizeof(result), "Class %d (%.2f%%)", max_class, max_prob * 100.0f);
    return result; 
}

// Image Recognition Cleanup
void image_recognition_cleanup() {
    // Clean up TensorFlow Lite resources
    cleanup_tflite(); // Call the cleanup function in your TFLite wrapper
}
