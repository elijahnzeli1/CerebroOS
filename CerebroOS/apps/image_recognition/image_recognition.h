#ifndef IMAGE_RECOGNITION_H
#define IMAGE_RECOGNITION_H

#include <stdint.h>

// Error Codes for Image Recognition
typedef enum {
    IMAGE_RECOGNITION_ERROR_NONE = 0,         // No error
    IMAGE_RECOGNITION_ERROR_TFLITE_INIT,      // TensorFlow Lite initialization error
    IMAGE_RECOGNITION_ERROR_INVALID_IMAGE,    // Invalid image data
    IMAGE_RECOGNITION_ERROR_INFERENCE_FAILED, // Model inference failed
    // ... Add more specific error codes as needed
} ImageRecognitionError;

// Image Configuration (Optional)
typedef struct {
    int width;      // Image width
    int height;     // Image height
    int channels;   // Number of color channels (e.g., 3 for RGB)
    // Add other image-related configuration parameters as needed.
} ImageConfig;

// Function Declarations

// Initialize the image recognition module
// Returns an ImageRecognitionError indicating success or failure
ImageRecognitionError image_recognition_init(const ImageConfig* config);

// Classify an image
// Parameters:
//   - image_data: Pointer to raw image data
//   - width: Width of the image in pixels
//   - height: Height of the image in pixels
// Returns:
//   - A null-terminated string containing the predicted class label, or an error string if an error occurred.
const char* image_recognition_classify(const uint8_t* image_data, int width, int height);

// Clean up resources used by the image recognition module
void image_recognition_cleanup();

// Optional Functions (Consider adding these for more flexibility)

// Set image recognition model (if you want to support multiple models)
ImageRecognitionError image_recognition_set_model(const char* modelPath);

// Get a list of available models
ImageRecognitionError image_recognition_get_available_models(char** models, int maxModels);

#endif // IMAGE_RECOGNITION_H
