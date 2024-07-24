#include "ui_manager.h"
#include "../drivers/display_driver.h"
#include <stdlib.h>
#include <string.h>

#define MAX_BUTTONS 10

static Button buttons[MAX_BUTTONS];
static int button_count = 0;
static bool redraw_needed = true;  // Track if a redraw is necessary

// Button Styling Constants
#define BUTTON_WIDTH 80  // Fixed width for consistency
#define BUTTON_HEIGHT 25 // Fixed height for consistency
#define BUTTON_MARGIN 5  // Margin between buttons
#define TEXT_OFFSET_X 10 // Text offset within buttons
#define TEXT_OFFSET_Y 6  // Text offset within buttons

// Color Constants
#define COLOR_BACKGROUND 0xFFFF
#define COLOR_HEADER 0x001F
#define COLOR_BUTTON 0xF800
#define COLOR_BUTTON_PRESSED 0x7BE0 // A slightly darker shade for pressed buttons
#define COLOR_TEXT 0x0000

void ui_init() {
    button_count = 0;
    redraw_needed = true; // Initial draw on startup
}

void ui_draw() {
    if (!redraw_needed) {
        return; // No need to redraw if nothing has changed
    }

    display_clear(COLOR_BACKGROUND);

    // Draw header
    display_draw_rect(0, 0, 240, 30, COLOR_HEADER);
    display_draw_text(10, 10, "CerebroOS", COLOR_TEXT);

    // Draw buttons with better positioning and a pressed state
    int y = 30 + BUTTON_MARGIN; // Start below header
    for (int i = 0; i < button_count; i++) {
        Button* btn = &buttons[i];
        int x = (240 - BUTTON_WIDTH) / 2; // Center buttons horizontally
        uint16_t color = btn->pressed ? COLOR_BUTTON_PRESSED : COLOR_BUTTON;
        display_draw_rect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, color);
        display_draw_text(x + TEXT_OFFSET_X, y + TEXT_OFFSET_Y, btn->text, COLOR_TEXT);
        y += BUTTON_HEIGHT + BUTTON_MARGIN;
    }

    display_update();
    redraw_needed = false; // Mark that we've redrawn
}

void ui_handle_click(int x, int y) {
    for (int i = 0; i < button_count; i++) {
        Button* btn = &buttons[i];
        if (x >= btn->x && x < btn->x + BUTTON_WIDTH &&
            y >= btn->y && y < btn->y + BUTTON_HEIGHT) {

            // Visual feedback for the press
            btn->pressed = true;
            redraw_needed = true;  // Force a redraw to show the pressed state
            ui_draw();             

            if (btn->on_click) {
                btn->on_click();
            }

            // Release after a short delay
            // (You'll likely want to replace this with an actual timer)
            for (volatile int delay = 0; delay < 100000; delay++) {} 

            btn->pressed = false;
            redraw_needed = true; // Redraw to show the unpressed state
            ui_draw();  

            break; 
        }
    }
}

void ui_add_button(int x, int y, int width, int height, const char* text, void (*on_click)(void)) {
    if (button_count < MAX_BUTTONS) {
        Button* btn = &buttons[button_count++];
        btn->x = x;
        btn->y = y;
        btn->width = width;
        btn->height = height;
        btn->text = text;
        btn->on_click = on_click;
    }
}