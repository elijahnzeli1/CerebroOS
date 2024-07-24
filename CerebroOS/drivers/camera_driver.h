#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Error Codes for Camera Operations
typedef enum {
    CAMERA_ERROR_NONE = 0,         // No error
    CAMERA_ERROR_SDL_INIT,         // SDL initialization error
    CAMERA_ERROR_WINDOW,           // Window creation error
    CAMERA_ERROR_RENDERER,         // Renderer creation error
    CAMERA_ERROR_TEXTURE,          // Texture creation error
    CAMERA_ERROR_MEMORY,           // Memory allocation error
    CAMERA_ERROR_HARDWARE,         // Camera hardware error
    CAMERA_ERROR_NOT_ACTIVE,       // Camera not active
    // ... add more error codes as needed
} CameraError;

// Camera Configuration Structure
typedef struct CameraConfig {
    int width;                  // Camera preview width (in pixels)
    int height;                 // Camera preview height (in pixels)
    // ... other configuration parameters (e.g., frame rate, format)
} CameraConfig;

// Camera Information Structure
typedef struct CameraInfo {
    int width;                  // Actual camera sensor width
    int height;                 // Actual camera sensor height
    // ... other camera information (e.g., supported formats, capabilities)
} CameraInfo;

// Function Declarations

// Initialize the camera driver
CameraError camera_init(const CameraConfig* config);

// Get information about the camera
CameraError camera_get_info(CameraInfo* info);

// Start capturing frames from the camera
CameraError camera_start_capture();

// Stop capturing frames from the camera
CameraError camera_stop_capture();

// Capture a frame from the camera
//   - buffer: Buffer to store the captured frame data (optional)
//   - buffer_size: Size of the provided buffer (in bytes)
// Returns:
//   - CAMERA_ERROR_NONE if successful, or an error code on failure
CameraError camera_capture(uint8_t* buffer, size_t buffer_size);

// Set the camera preview callback function
//   - callback: A function pointer to be called when a new frame is available
//   - userdata: Optional user data to pass to the callback function
void camera_set_preview_callback(void (*callback)(const uint8_t* frameData, size_t dataSize, void* userdata),
                                void* userdata);

// Cleanup the camera driver (release resources)
void camera_cleanup();

#endif // CAMERA_DRIVER_H
