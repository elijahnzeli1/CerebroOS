#include "../../os/app_framework.h"
#include "../../os/ui_framework.h"
#include "../../os/network.h"
#include "../../os/security.h"
#include <string.h>

// App state
typedef struct {
    UiWindow* main_window;
    UiListBox* conversation_list;
    UiTextBox* message_input;
    UiButton* send_button;
    char current_contact[32];
    SecureStorage* message_storage;
} MessagingApp;

static MessagingApp app_state;

// Message structure
typedef struct {
    char sender[32];
    char recipient[32];
    char content[160];
    uint32_t timestamp;
    bool read;
} Message;

// Callback when send button is clicked
static void on_send_click(UiElement* element) {
    const char* message_text = app_state.message_input->text;
    if (strlen(message_text) == 0 || strlen(app_state.current_contact) == 0) {
        return;
    }

    // Create SMS message
    SmsMessage sms = {
        .flash_message = false,
        .priority = 1
    };
    strncpy(sms.recipient, app_state.current_contact, sizeof(sms.recipient) - 1);
    strncpy(sms.message, message_text, sizeof(sms.message) - 1);

    // Check permissions
    if (!security_has_permission("messaging_app", PERM_SMS_SEND)) {
        if (!security_request_permission("messaging_app", PERM_SMS_SEND)) {
            return;
        }
    }

    // Send message
    if (sms_send(&sms)) {
        // Store message in secure storage
        Message msg = {
            .timestamp = /* Get current time */,
            .read = true
        };
        strncpy(msg.sender, "Me", sizeof(msg.sender) - 1);
        strncpy(msg.recipient, app_state.current_contact, sizeof(msg.recipient) - 1);
        strncpy(msg.content, message_text, sizeof(msg.content) - 1);
        
        secure_storage_write(app_state.message_storage, &msg, sizeof(Message));
        
        // Clear input
        app_state.message_input->text[0] = '\0';
        app_state.message_input->text_length = 0;
        ui_invalidate((UiElement*)app_state.message_input);
    }
}

// Initialize UI
static void init_ui(void) {
    // Create main window
    app_state.main_window = ui_create_window(
        "Messages",
        0, 0, 240, 320  // Typical feature phone resolution
    );

    // Create conversation list
    app_state.conversation_list = ui_create_listbox(
        (UiElement*)app_state.main_window,
        5, 5, 230, 200
    );

    // Create message input
    app_state.message_input = ui_create_textbox(
        (UiElement*)app_state.main_window,
        5, 240, 180, 30
    );
    app_state.message_input->multiline = false;

    // Create send button
    app_state.send_button = ui_create_button(
        (UiElement*)app_state.main_window,
        190, 240, 45, 30,
        "Send"
    );
    app_state.send_button->base.on_click = on_send_click;
}

// Initialize secure storage
static void init_storage(void) {
    app_state.message_storage = secure_storage_create("messages", 1024 * 1024);  // 1MB for messages
    if (!app_state.message_storage) {
        // Handle error
        return;
    }
}

// App lifecycle callbacks
static void on_create(void) {
    memset(&app_state, 0, sizeof(MessagingApp));
    
    // Initialize components
    init_ui();
    init_storage();
    
    // Request permissions
    security_request_permission("messaging_app", PERM_SMS_SEND);
    security_request_permission("messaging_app", PERM_SMS_RECEIVE);
    security_request_permission("messaging_app", PERM_CONTACTS_READ);
}

static void on_destroy(void) {
    // Cleanup
    if (app_state.message_storage) {
        secure_storage_destroy(app_state.message_storage);
    }
}

static void on_pause(void) {
    // Save state if needed
}

static void on_resume(void) {
    // Refresh messages
}

// Register app
APP_DECLARE("messaging_app") {
    .on_create = on_create,
    .on_destroy = on_destroy,
    .on_pause = on_pause,
    .on_resume = on_resume,
    .required_permissions = PERM_SMS_SEND | PERM_SMS_RECEIVE | PERM_CONTACTS_READ
};
