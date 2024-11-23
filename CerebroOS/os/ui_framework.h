#ifndef UI_FRAMEWORK_H
#define UI_FRAMEWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

// UI element types
typedef enum {
    UI_ELEMENT_WINDOW,
    UI_ELEMENT_BUTTON,
    UI_ELEMENT_LABEL,
    UI_ELEMENT_TEXTBOX,
    UI_ELEMENT_LISTBOX,
    UI_ELEMENT_CHECKBOX,
    UI_ELEMENT_PROGRESS,
    UI_ELEMENT_ICON
} UiElementType;

// UI element states
typedef enum {
    UI_STATE_NORMAL,
    UI_STATE_FOCUSED,
    UI_STATE_PRESSED,
    UI_STATE_DISABLED
} UiElementState;

// UI colors (16-bit RGB565 format)
typedef uint16_t UiColor;
#define UI_COLOR_BLACK   0x0000
#define UI_COLOR_WHITE   0xFFFF
#define UI_COLOR_RED     0xF800
#define UI_COLOR_GREEN   0x07E0
#define UI_COLOR_BLUE    0x001F
#define UI_COLOR_GRAY    0x7BEF

// UI rectangle
typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} UiRect;

// UI element base structure
typedef struct UiElement {
    UiElementType type;
    UiElementState state;
    UiRect rect;
    UiColor bg_color;
    UiColor fg_color;
    bool visible;
    bool enabled;
    struct UiElement* parent;
    struct UiElement* first_child;
    struct UiElement* next_sibling;
    void (*on_paint)(struct UiElement* element);
    void (*on_input)(struct UiElement* element, const InputEvent* event);
    void (*on_focus)(struct UiElement* element, bool focused);
    void* user_data;
} UiElement;

// UI window (top-level container)
typedef struct {
    UiElement base;
    char title[32];
    bool fullscreen;
    bool modal;
} UiWindow;

// UI button
typedef struct {
    UiElement base;
    char text[32];
    bool pressed;
    void (*on_click)(struct UiElement* element);
} UiButton;

// UI label
typedef struct {
    UiElement base;
    char text[64];
    bool word_wrap;
    uint8_t text_alignment;
} UiLabel;

// UI textbox
typedef struct {
    UiElement base;
    char* text;
    size_t text_capacity;
    size_t text_length;
    size_t cursor_pos;
    bool password_mode;
    bool multiline;
    void (*on_text_changed)(struct UiElement* element);
} UiTextBox;

// UI listbox
typedef struct {
    UiElement base;
    char** items;
    size_t item_count;
    size_t selected_index;
    uint8_t visible_items;
    void (*on_selection_changed)(struct UiElement* element);
} UiListBox;

// UI Framework functions
bool ui_init(void);
void ui_shutdown(void);

// Element creation
UiWindow* ui_create_window(const char* title, int16_t x, int16_t y, uint16_t width, uint16_t height);
UiButton* ui_create_button(UiElement* parent, const char* text, int16_t x, int16_t y, uint16_t width, uint16_t height);
UiLabel* ui_create_label(UiElement* parent, const char* text, int16_t x, int16_t y, uint16_t width, uint16_t height);
UiTextBox* ui_create_textbox(UiElement* parent, int16_t x, int16_t y, uint16_t width, uint16_t height);
UiListBox* ui_create_listbox(UiElement* parent, int16_t x, int16_t y, uint16_t width, uint16_t height);

// Element management
void ui_destroy_element(UiElement* element);
void ui_set_visible(UiElement* element, bool visible);
void ui_set_enabled(UiElement* element, bool enabled);
void ui_set_focus(UiElement* element);
UiElement* ui_get_focus(void);

// Layout and drawing
void ui_invalidate(UiElement* element);
void ui_invalidate_rect(const UiRect* rect);
void ui_layout(UiElement* element);
void ui_paint(UiElement* element);

// Input handling
void ui_handle_input(const InputEvent* event);

// Theme management
typedef struct {
    UiColor window_bg;
    UiColor window_fg;
    UiColor button_bg;
    UiColor button_fg;
    UiColor button_bg_pressed;
    UiColor text_color;
    UiColor selection_color;
    uint8_t font_size;
    uint8_t padding;
    uint8_t border_width;
} UiTheme;

void ui_set_theme(const UiTheme* theme);
const UiTheme* ui_get_theme(void);

// Animation support
typedef struct {
    UiElement* element;
    uint32_t duration;
    uint32_t start_time;
    void (*update)(struct UiAnimation* anim, float progress);
    void (*complete)(struct UiAnimation* anim);
    void* user_data;
} UiAnimation;

UiAnimation* ui_create_animation(UiElement* element, uint32_t duration);
void ui_start_animation(UiAnimation* anim);
void ui_stop_animation(UiAnimation* anim);
void ui_update_animations(void);

#endif // UI_FRAMEWORK_H
