#include "camera_driver.h"
#include <SDL2/SDL.h>
#include <stdbool.h> // for boolean types

#define CAMERA_WIDTH 320    // Reduced resolution for small screens
#define CAMERA_HEIGHT 240   // Reduced resolution for small screens

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint8_t* buffer = NULL;   // Buffer for camera data
static size_t buffer_size = 0;  // Size of the buffer
static bool camera_active = false; // Flag to track camera state


// Camera Initialization
CameraError camera_init() {
    // SDL Video Initialization (with error checking)
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return CAMERA_ERROR_SDL_INIT;
    }

    // Create Window (Smaller size for small screens, check for errors)
    window = SDL_CreateWindow("Camera Preview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              CAMERA_WIDTH, CAMERA_HEIGHT, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_VIDEO); 
        return CAMERA_ERROR_WINDOW;
    }

    // Create Renderer (Prefer hardware acceleration)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        camera_cleanup(); // Clean up resources
        return CAMERA_ERROR_RENDERER;
    }

    // Create Texture (Streaming for direct pixel access)
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
                                CAMERA_WIDTH, CAMERA_HEIGHT);
    if (!texture) {
        SDL_Log("SDL_CreateTexture Error: %s", SDL_GetError());
        camera_cleanup();
        return CAMERA_ERROR_TEXTURE;
    }

    // Calculate buffer size
    buffer_size = CAMERA_WIDTH * CAMERA_HEIGHT * 3; // 3 bytes per pixel (RGB)

    // Allocate pixel buffer
    buffer = (uint8_t*)malloc(buffer_size);
    if (!buffer) {
        SDL_Log("Pixel buffer allocation failed!");
        camera_cleanup();
        return CAMERA_ERROR_MEMORY;
    }

    return CAMERA_ERROR_NONE; // No errors
}

// Start Camera Capture
CameraError camera_start_capture() {
    // Start the camera (replace with your actual camera initialization)
    if (init_camera_hardware() != 0) {
        return CAMERA_ERROR_HARDWARE; // Error starting camera
    }
    camera_active = true;  // Mark camera as active
    return CAMERA_ERROR_NONE;
}


// Stop Camera Capture
CameraError camera_stop_capture() {
    // Stop the camera (replace with your actual camera deactivation)
    deinit_camera_hardware();
    camera_active = false; // Mark camera as inactive
    return CAMERA_ERROR_NONE;
}


// Camera Capture (Updated to get data from camera)
CameraError camera_capture(uint8_t* user_buffer, size_t user_buffer_size) {
    if (!camera_active) {
        return CAMERA_ERROR_NOT_ACTIVE; // Camera not started
    }

    // Capture data from camera hardware (replace with your actual implementation)
    if (get_frame_from_camera(buffer, buffer_size) != 0) {
        return CAMERA_ERROR_HARDWARE; // Error capturing frame
    }

    // Copy data to user buffer if provided
    if (user_buffer && user_buffer_size >= buffer_size) {
        memcpy(user_buffer, buffer, buffer_size);
    }

    // Update texture with new data
    SDL_UpdateTexture(texture, NULL, buffer, CAMERA_WIDTH * 3);

    // Render the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    return CAMERA_ERROR_NONE;
}


// Camera Cleanup
void camera_cleanup() {
    // Free pixel buffer and destroy SDL objects (with proper order)
    if (buffer) free(buffer);
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    // Clean up camera hardware (replace with your actual implementation)
    deinit_camera_hardware();

    // Quit SDL video subsystem
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
