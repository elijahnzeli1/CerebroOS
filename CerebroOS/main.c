#include "os/cerebro_os.h"
#include "drivers/display_driver.h"
#include "ui/ui_manager.h"
#include "network/network_manager.h"
#include "update/ota_update.h"
#include <SDL2/SDL.h>
#include <stdio.h>

// Window Dimensions (Adjust for your device)
#define WINDOW_WIDTH 240
#define WINDOW_HEIGHT 320

// Button Action Structure (defined in ui_manager.c)
extern typedef struct {
    const char* name;           
    void (*action)(void*);      
    void* data;                
} ButtonAction;

// Callback Actions (defined in ui_manager.c)
extern ButtonAction updateAction;
extern ButtonAction app1Action;
extern ButtonAction app2Action;
extern ButtonAction app3Action;

int main() {
    // SDL Initialization
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create Window
    SDL_Window* window = SDL_CreateWindow("CerebroOS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Get Window Surface
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    // Initialize CerebroOS Components
    display_init(screenSurface);  // Pass the surface to the display driver
    ui_init();
    network_init();

    // UI Placement and Styling
    const int buttonWidth = 60;
    const int buttonHeight = 40;
    const int spacing = 10;
    int startY = (WINDOW_HEIGHT - buttonHeight * 4 - spacing * 3) / 2; // Center buttons

    // Add Buttons with Actions
    ui_add_button(spacing, startY, buttonWidth, buttonHeight, &app1Action);
    ui_add_button(spacing * 2 + buttonWidth, startY, buttonWidth, buttonHeight, &app2Action);
    ui_add_button(spacing, startY + buttonHeight + spacing, buttonWidth, buttonHeight, &app3Action);
    ui_add_button(spacing * 2 + buttonWidth, startY + buttonHeight + spacing, buttonWidth, buttonHeight, &updateAction);

    // Network Connection (Example)
    network_connect(NETWORK_WIFI);

    // Main Event Loop
    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    ui_handle_click(event.button.x, event.button.y); 
                    break;
            }
        }

        // Run OS Tasks
        os_run(); 

        // Draw UI
        ui_draw(); 

        // Update Display
        SDL_UpdateWindowSurface(window);
    }

    // Cleanup
    display_cleanup();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
