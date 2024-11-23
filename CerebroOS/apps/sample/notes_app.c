#include "../../os/app_framework.h"
#include "../../os/ui_framework.h"
#include "../../os/security.h"
#include <string.h>
#include <stdio.h>

#define MAX_NOTE_LENGTH 1024
#define MAX_TITLE_LENGTH 64
#define MAX_NOTES 100

// Note structure
typedef struct {
    char title[MAX_TITLE_LENGTH];
    char content[MAX_NOTE_LENGTH];
    uint32_t created_time;
    uint32_t modified_time;
    bool encrypted;
} Note;

// App state
typedef struct {
    UiWindow* main_window;
    UiWindow* edit_window;
    UiListBox* notes_list;
    UiTextBox* title_input;
    UiTextBox* content_input;
    UiButton* new_button;
    UiButton* save_button;
    UiButton* delete_button;
    SecureStorage* notes_storage;
    Note* current_note;
    size_t note_count;
    bool editing;
} NotesApp;

static NotesApp app_state;

// Animation for transitioning between windows
static void animate_window_transition(UiWindow* from, UiWindow* to, bool forward) {
    UiAnimation* anim = ui_create_animation((UiElement*)from, 300);  // 300ms duration
    
    // Define animation update function
    anim->update = [](UiAnimation* anim, float progress) {
        UiWindow* from = (UiWindow*)anim->element;
        UiWindow* to = (UiWindow*)anim->user_data;
        
        // Slide animation
        int16_t screen_width = 240;  // Feature phone screen width
        float from_x = forward ? -progress * screen_width : progress * screen_width;
        float to_x = forward ? (1 - progress) * screen_width : -(1 - progress) * screen_width;
        
        from->base.rect.x = (int16_t)from_x;
        to->base.rect.x = (int16_t)to_x;
        
        ui_invalidate((UiElement*)from);
        ui_invalidate((UiElement*)to);
    };
    
    // Start animation
    anim->user_data = to;
    ui_start_animation(anim);
}

// Load notes from storage
static void load_notes(void) {
    if (!app_state.notes_storage) {
        return;
    }
    
    // Read note count
    secure_storage_read(app_state.notes_storage, &app_state.note_count, sizeof(size_t));
    
    // Clear and populate list
    ui_listbox_clear(app_state.notes_list);
    
    for (size_t i = 0; i < app_state.note_count && i < MAX_NOTES; i++) {
        Note note;
        if (secure_storage_read(app_state.notes_storage, &note, sizeof(Note))) {
            ui_listbox_add_item(app_state.notes_list, note.title);
        }
    }
}

// Save current note
static void save_note(void) {
    if (!app_state.current_note || !app_state.notes_storage) {
        return;
    }
    
    // Update note data
    strncpy(app_state.current_note->title, 
            app_state.title_input->text,
            MAX_TITLE_LENGTH - 1);
    
    strncpy(app_state.current_note->content,
            app_state.content_input->text,
            MAX_NOTE_LENGTH - 1);
    
    app_state.current_note->modified_time = /* Get current time */;
    
    // Save to storage
    size_t note_index = app_state.notes_list->selected_index;
    size_t offset = sizeof(size_t) + (note_index * sizeof(Note));
    secure_storage_write(app_state.notes_storage, app_state.current_note, sizeof(Note));
    
    // Update list
    ui_listbox_set_item(app_state.notes_list, 
                        note_index,
                        app_state.current_note->title);
}

// Button callbacks
static void on_new_click(UiElement* element) {
    app_state.editing = true;
    app_state.current_note = calloc(1, sizeof(Note));
    
    // Clear inputs
    app_state.title_input->text[0] = '\0';
    app_state.title_input->text_length = 0;
    app_state.content_input->text[0] = '\0';
    app_state.content_input->text_length = 0;
    
    // Show edit window with animation
    animate_window_transition(app_state.main_window,
                            app_state.edit_window,
                            true);
}

static void on_save_click(UiElement* element) {
    save_note();
    app_state.editing = false;
    
    // Return to main window with animation
    animate_window_transition(app_state.edit_window,
                            app_state.main_window,
                            false);
}

static void on_delete_click(UiElement* element) {
    size_t note_index = app_state.notes_list->selected_index;
    if (note_index >= app_state.note_count) {
        return;
    }
    
    // Remove note and shift others up
    size_t offset = sizeof(size_t) + (note_index * sizeof(Note));
    for (size_t i = note_index; i < app_state.note_count - 1; i++) {
        Note next_note;
        secure_storage_read(app_state.notes_storage,
                          &next_note,
                          sizeof(Note));
        secure_storage_write(app_state.notes_storage,
                           &next_note,
                           sizeof(Note));
    }
    
    app_state.note_count--;
    secure_storage_write(app_state.notes_storage,
                        &app_state.note_count,
                        sizeof(size_t));
    
    // Update list
    load_notes();
}

// Initialize UI
static void init_ui(void) {
    // Create main window
    app_state.main_window = ui_create_window(
        "Notes",
        0, 0, 240, 320
    );
    
    // Create notes list
    app_state.notes_list = ui_create_listbox(
        (UiElement*)app_state.main_window,
        5, 5, 230, 250
    );
    
    // Create new button
    app_state.new_button = ui_create_button(
        (UiElement*)app_state.main_window,
        5, 260, 70, 30,
        "New"
    );
    app_state.new_button->base.on_click = on_new_click;
    
    // Create delete button
    app_state.delete_button = ui_create_button(
        (UiElement*)app_state.main_window,
        165, 260, 70, 30,
        "Delete"
    );
    app_state.delete_button->base.on_click = on_delete_click;
    
    // Create edit window (initially off-screen)
    app_state.edit_window = ui_create_window(
        "Edit Note",
        240, 0, 240, 320  // Start off-screen
    );
    
    // Create title input
    app_state.title_input = ui_create_textbox(
        (UiElement*)app_state.edit_window,
        5, 5, 230, 30
    );
    app_state.title_input->multiline = false;
    
    // Create content input
    app_state.content_input = ui_create_textbox(
        (UiElement*)app_state.edit_window,
        5, 40, 230, 215
    );
    app_state.content_input->multiline = true;
    
    // Create save button
    app_state.save_button = ui_create_button(
        (UiElement*)app_state.edit_window,
        85, 260, 70, 30,
        "Save"
    );
    app_state.save_button->base.on_click = on_save_click;
}

// Initialize storage
static void init_storage(void) {
    app_state.notes_storage = secure_storage_create("notes", 
        sizeof(size_t) + (MAX_NOTES * sizeof(Note)));
    
    if (app_state.notes_storage) {
        load_notes();
    }
}

// App lifecycle callbacks
static void on_create(void) {
    memset(&app_state, 0, sizeof(NotesApp));
    
    // Initialize components
    init_ui();
    init_storage();
    
    // Request permissions
    security_request_permission("notes_app", PERM_STORAGE_READ);
    security_request_permission("notes_app", PERM_STORAGE_WRITE);
}

static void on_destroy(void) {
    if (app_state.notes_storage) {
        secure_storage_destroy(app_state.notes_storage);
    }
    
    if (app_state.current_note) {
        free(app_state.current_note);
    }
}

static void on_pause(void) {
    if (app_state.editing) {
        save_note();
    }
}

static void on_resume(void) {
    load_notes();
}

// Register app
APP_DECLARE("notes_app") {
    .on_create = on_create,
    .on_destroy = on_destroy,
    .on_pause = on_pause,
    .on_resume = on_resume,
    .required_permissions = PERM_STORAGE_READ | PERM_STORAGE_WRITE
};
