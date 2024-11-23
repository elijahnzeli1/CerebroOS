#include "../../os/app_framework.h"
#include "../../os/ui_framework.h"
#include "../../os/network.h"
#include "../../os/security.h"
#include <string.h>
#include <stdio.h>

// Call states
typedef enum {
    CALL_STATE_IDLE,
    CALL_STATE_DIALING,
    CALL_STATE_RINGING,
    CALL_STATE_ACTIVE,
    CALL_STATE_HOLDING,
    CALL_STATE_ENDED
} CallState;

// Contact structure
typedef struct {
    char name[64];
    char number[32];
    char email[64];
    bool favorite;
} Contact;

// Call structure
typedef struct {
    char number[32];
    char name[64];
    uint32_t start_time;
    uint32_t duration;
    CallState state;
    bool is_incoming;
    bool is_video;
    bool is_muted;
    bool is_speaker_on;
} Call;

// App state
typedef struct {
    // Windows
    UiWindow* main_window;
    UiWindow* dialer_window;
    UiWindow* contacts_window;
    UiWindow* call_window;
    
    // Dialer elements
    UiTextBox* number_input;
    UiButton* dial_buttons[12];
    UiButton* call_button;
    UiButton* delete_button;
    
    // Contact elements
    UiListBox* contacts_list;
    UiButton* add_contact_button;
    UiButton* search_button;
    UiTextBox* search_input;
    
    // Call elements
    UiLabel* caller_label;
    UiLabel* duration_label;
    UiButton* end_button;
    UiButton* mute_button;
    UiButton* speaker_button;
    UiButton* hold_button;
    
    // Navigation
    UiButton* dialer_tab;
    UiButton* contacts_tab;
    UiButton* history_tab;
    
    // State
    Call* current_call;
    Contact* contacts;
    size_t contact_count;
    SecureStorage* contacts_storage;
    uint32_t call_timer_id;
} PhoneApp;

static PhoneApp app_state;

// Update call duration display
static void update_call_duration(void) {
    if (!app_state.current_call || 
        app_state.current_call->state != CALL_STATE_ACTIVE) {
        return;
    }
    
    uint32_t current_time = /* Get current time */;
    uint32_t duration = current_time - app_state.current_call->start_time;
    
    char duration_text[32];
    snprintf(duration_text, sizeof(duration_text), 
             "%02u:%02u:%02u",
             duration / 3600,
             (duration % 3600) / 60,
             duration % 60);
    
    ui_set_text((UiElement*)app_state.duration_label, duration_text);
}

// Handle incoming call
static void handle_incoming_call(const char* number) {
    if (app_state.current_call) {
        // Already in a call, implement call waiting
        return;
    }
    
    app_state.current_call = calloc(1, sizeof(Call));
    strncpy(app_state.current_call->number, number, sizeof(app_state.current_call->number) - 1);
    app_state.current_call->state = CALL_STATE_RINGING;
    app_state.current_call->is_incoming = true;
    
    // Look up contact name
    for (size_t i = 0; i < app_state.contact_count; i++) {
        if (strcmp(app_state.contacts[i].number, number) == 0) {
            strncpy(app_state.current_call->name, 
                    app_state.contacts[i].name,
                    sizeof(app_state.current_call->name) - 1);
            break;
        }
    }
    
    // Show call window
    animate_window_transition(app_state.main_window,
                            app_state.call_window,
                            true);
    
    // Update UI
    char caller_text[128];
    snprintf(caller_text, sizeof(caller_text),
             "Incoming Call\n%s",
             app_state.current_call->name[0] ? 
             app_state.current_call->name : 
             app_state.current_call->number);
    ui_set_text((UiElement*)app_state.caller_label, caller_text);
}

// Start outgoing call
static void start_call(const char* number) {
    if (app_state.current_call) {
        return;
    }
    
    app_state.current_call = calloc(1, sizeof(Call));
    strncpy(app_state.current_call->number, number, sizeof(app_state.current_call->number) - 1);
    app_state.current_call->state = CALL_STATE_DIALING;
    app_state.current_call->is_incoming = false;
    
    // Look up contact name
    for (size_t i = 0; i < app_state.contact_count; i++) {
        if (strcmp(app_state.contacts[i].number, number) == 0) {
            strncpy(app_state.current_call->name, 
                    app_state.contacts[i].name,
                    sizeof(app_state.current_call->name) - 1);
            break;
        }
    }
    
    // Show call window
    animate_window_transition(app_state.main_window,
                            app_state.call_window,
                            true);
    
    // Update UI
    char caller_text[128];
    snprintf(caller_text, sizeof(caller_text),
             "Calling...\n%s",
             app_state.current_call->name[0] ? 
             app_state.current_call->name : 
             app_state.current_call->number);
    ui_set_text((UiElement*)app_state.caller_label, caller_text);
    
    // Start call via system
    // Implementation depends on hardware abstraction layer
}

// End current call
static void end_call(void) {
    if (!app_state.current_call) {
        return;
    }
    
    // End call via system
    // Implementation depends on hardware abstraction layer
    
    free(app_state.current_call);
    app_state.current_call = NULL;
    
    // Return to main window
    animate_window_transition(app_state.call_window,
                            app_state.main_window,
                            false);
}

// Button callbacks
static void on_dial_button_click(UiElement* element) {
    UiButton* button = (UiButton*)element;
    const char* digit = button->text;
    
    size_t current_length = strlen(app_state.number_input->text);
    if (current_length < 31) {
        app_state.number_input->text[current_length] = digit[0];
        app_state.number_input->text[current_length + 1] = '\0';
        app_state.number_input->text_length++;
        ui_invalidate((UiElement*)app_state.number_input);
    }
}

static void on_call_button_click(UiElement* element) {
    const char* number = app_state.number_input->text;
    if (strlen(number) > 0) {
        start_call(number);
    }
}

static void on_delete_button_click(UiElement* element) {
    size_t current_length = strlen(app_state.number_input->text);
    if (current_length > 0) {
        app_state.number_input->text[current_length - 1] = '\0';
        app_state.number_input->text_length--;
        ui_invalidate((UiElement*)app_state.number_input);
    }
}

static void on_end_button_click(UiElement* element) {
    end_call();
}

static void on_mute_button_click(UiElement* element) {
    if (app_state.current_call) {
        app_state.current_call->is_muted = !app_state.current_call->is_muted;
        // Implement mute functionality
    }
}

static void on_speaker_button_click(UiElement* element) {
    if (app_state.current_call) {
        app_state.current_call->is_speaker_on = !app_state.current_call->is_speaker_on;
        // Implement speaker functionality
    }
}

// Initialize UI
static void init_ui(void) {
    // Create main window with tabs
    app_state.main_window = ui_create_window(
        "Phone",
        0, 0, 240, 320
    );
    
    // Create tab buttons
    app_state.dialer_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        0, 0, 80, 30,
        "Dialer"
    );
    
    app_state.contacts_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        80, 0, 80, 30,
        "Contacts"
    );
    
    app_state.history_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        160, 0, 80, 30,
        "History"
    );
    
    // Create dialer window
    app_state.dialer_window = ui_create_window(
        "Dialer",
        0, 30, 240, 290
    );
    
    // Create number input
    app_state.number_input = ui_create_textbox(
        (UiElement*)app_state.dialer_window,
        5, 5, 230, 40
    );
    app_state.number_input->multiline = false;
    
    // Create dial pad
    const char* digits[] = {
        "1", "2", "3",
        "4", "5", "6",
        "7", "8", "9",
        "*", "0", "#"
    };
    
    for (int i = 0; i < 12; i++) {
        int row = i / 3;
        int col = i % 3;
        app_state.dial_buttons[i] = ui_create_button(
            (UiElement*)app_state.dialer_window,
            10 + (col * 75), 50 + (row * 60),
            70, 55,
            digits[i]
        );
        app_state.dial_buttons[i]->base.on_click = on_dial_button_click;
    }
    
    // Create call and delete buttons
    app_state.call_button = ui_create_button(
        (UiElement*)app_state.dialer_window,
        85, 290,
        70, 55,
        "Call"
    );
    app_state.call_button->base.on_click = on_call_button_click;
    
    app_state.delete_button = ui_create_button(
        (UiElement*)app_state.dialer_window,
        165, 290,
        70, 55,
        "Del"
    );
    app_state.delete_button->base.on_click = on_delete_button_click;
    
    // Create call window
    app_state.call_window = ui_create_window(
        "Call",
        240, 0, 240, 320  // Start off-screen
    );
    
    // Create call UI elements
    app_state.caller_label = ui_create_label(
        (UiElement*)app_state.call_window,
        5, 5, 230, 60
    );
    
    app_state.duration_label = ui_create_label(
        (UiElement*)app_state.call_window,
        5, 70, 230, 30
    );
    
    // Create call control buttons
    app_state.end_button = ui_create_button(
        (UiElement*)app_state.call_window,
        85, 250,
        70, 55,
        "End"
    );
    app_state.end_button->base.on_click = on_end_button_click;
    
    app_state.mute_button = ui_create_button(
        (UiElement*)app_state.call_window,
        5, 250,
        70, 55,
        "Mute"
    );
    app_state.mute_button->base.on_click = on_mute_button_click;
    
    app_state.speaker_button = ui_create_button(
        (UiElement*)app_state.call_window,
        165, 250,
        70, 55,
        "Speaker"
    );
    app_state.speaker_button->base.on_click = on_speaker_button_click;
}

// Initialize storage
static void init_storage(void) {
    app_state.contacts_storage = secure_storage_create("contacts", 
        sizeof(size_t) + (1000 * sizeof(Contact)));  // Store up to 1000 contacts
    
    if (app_state.contacts_storage) {
        // Load contacts
        secure_storage_read(app_state.contacts_storage,
                          &app_state.contact_count,
                          sizeof(size_t));
        
        app_state.contacts = calloc(app_state.contact_count, sizeof(Contact));
        secure_storage_read(app_state.contacts_storage,
                          app_state.contacts,
                          app_state.contact_count * sizeof(Contact));
    }
}

// App lifecycle callbacks
static void on_create(void) {
    memset(&app_state, 0, sizeof(PhoneApp));
    
    // Initialize components
    init_ui();
    init_storage();
    
    // Request permissions
    security_request_permission("phone_app", PERM_CONTACTS_READ);
    security_request_permission("phone_app", PERM_CONTACTS_WRITE);
}

static void on_destroy(void) {
    if (app_state.contacts_storage) {
        secure_storage_destroy(app_state.contacts_storage);
    }
    
    if (app_state.contacts) {
        free(app_state.contacts);
    }
    
    if (app_state.current_call) {
        end_call();
    }
}

static void on_pause(void) {
    // Save any state if needed
}

static void on_resume(void) {
    // Refresh UI if needed
}

// Register app
APP_DECLARE("phone_app") {
    .on_create = on_create,
    .on_destroy = on_destroy,
    .on_pause = on_pause,
    .on_resume = on_resume,
    .required_permissions = PERM_CONTACTS_READ | PERM_CONTACTS_WRITE
};
