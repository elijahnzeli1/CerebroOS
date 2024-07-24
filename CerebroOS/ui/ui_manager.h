#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <stdbool.h>  // For boolean types (true/false)

// Forward declaration for the UI manager class
class UIManager;

// Button structure with improved flexibility
typedef struct Button {
    int x, y;                 // Position
    int width, height;        // Dimensions
    const char* text;          // Text label
    void (*on_click)(void);    // Callback function (optional)

    // Add additional properties for styling/behavior:
    bool isVisible;            // Is the button currently visible?
    bool isEnabled;           // Is the button enabled (can be clicked)?
    bool isPressed;           // Is the button currently being pressed?

    // Add a pointer to the UI manager for internal use
    UIManager* uiManager;     

} Button;

// UI Manager Class Declaration
class UIManager {
public:
    // Constructor (could take display dimensions as arguments)
    UIManager();

    // Add a button to the UI (returns a pointer to the created button)
    Button* addButton(int x, int y, int width, int height, const char* text, 
                      void (*on_click)(void) = nullptr);

    // Draw the UI elements to the display
    void draw();

    // Handle a touch event (x, y coordinates of the touch)
    void handleTouch(int x, int y);

private:
    // ... private members for storing button list, screen dimensions, etc.
    Button* buttons;          // Array or other container to store buttons
    int maxButtons;           // Maximum number of buttons allowed
    int numButtons;           // Number of buttons currently in use
};

#endif // UI_MANAGER_H
