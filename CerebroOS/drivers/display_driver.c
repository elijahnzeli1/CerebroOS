#include "display_driver.h"
#include <SDL2/SDL.h>
#include <string.h>  // for memset

// Display Dimensions
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint32_t* pixels = NULL;

// Initialize Display
void display_init() {
    // SDL Initialization (Error checking)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        exit(1);  // Exit on error
    }

    // Create Window (Error checking and reduced flags for performance)
    window = SDL_CreateWindow("CerebroOS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                              DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Create Renderer (Prefer hardware acceleration)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // Create Texture (Streaming for direct pixel access)
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!texture) {
        SDL_Log("SDL_CreateTexture Error: %s", SDL_GetError());
        // ... clean up SDL and exit 
        exit(1);
    }

    // Allocate Pixel Buffer (Error checking)
    pixels = (uint32_t*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint32_t));
    if (!pixels) {
        SDL_Log("Pixel buffer allocation failed!");
        // ... clean up SDL and exit 
        exit(1);
    }
}

// Clear Display (Optimized with memset)
void display_clear(uint16_t color) {
    uint32_t color32 = ((color & 0xF800) << 8) | ((color & 0x07E0) << 5) | ((color & 0x001F) << 3);
    memset(pixels, color32, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint32_t));
}

// Draw Pixel
void display_draw_pixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        uint32_t color32 = ((color & 0xF800) << 8) | ((color & 0x07E0) << 5) | ((color & 0x001F) << 3);
        pixels[y * DISPLAY_WIDTH + x] = color32;
    }
}

// Draw Rectangle
void display_draw_rect(int x, int y, int width, int height, uint16_t color) {
    // Clip rectangle to screen bounds for safety
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > DISPLAY_WIDTH) { width = DISPLAY_WIDTH - x; }
    if (y + height > DISPLAY_HEIGHT) { height = DISPLAY_HEIGHT - y; }

    // Optimized drawing (consider memset for large rectangles)
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            display_draw_pixel(x + dx, y + dy, color);
        }
    }
}

// Draw Text (placeholder - replace with a real font renderer!)
// ... (similar to your implementation or use an external font library)

// Update Display
void display_update() {
    SDL_UpdateTexture(texture, NULL, pixels, DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer); // Clear before rendering
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer); 
}

// Cleanup Display
void display_cleanup() {
    // Free pixel buffer and destroy SDL objects (with proper order)
    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
