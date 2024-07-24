#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h> // for uint16_t

// Forward declare a struct to represent display information
typedef struct DisplayInfo DisplayInfo;

// Error codes for display driver operations
typedef enum {
    DISPLAY_ERROR_NONE = 0,   // No error
    DISPLAY_ERROR_INIT,       // Initialization failure
    DISPLAY_ERROR_OUT_OF_BOUNDS,  // Coordinates outside display bounds
    // ... add more error codes as needed
} DisplayError;

// Initialization function (now returns an error code)
DisplayError display_init(DisplayInfo* info); 

// Get display information (dimensions, pixel format, etc.)
DisplayError display_get_info(DisplayInfo* info);

// Clearing functions (returns an error code)
DisplayError display_clear(uint16_t color);
DisplayError display_clear_region(int x, int y, int width, int height, uint16_t color);

// Drawing functions (returns an error code)
DisplayError display_draw_pixel(int x, int y, uint16_t color);
DisplayError display_draw_rect(int x, int y, int width, int height, uint16_t color);
DisplayError display_draw_text(int x, int y, const char* text, uint16_t color);
// ... add more drawing functions (e.g., lines, circles)

// Update the display (flushes changes to the screen)
DisplayError display_update();

// Cleanup the display driver (release resources)
DisplayError display_cleanup();

#endif // DISPLAY_DRIVER_H
