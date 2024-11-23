#include "../../os/app_framework.h"
#include "../../os/ui_framework.h"
#include "../../os/security.h"
#include "../../os/hal/clock_hal.h"

#define MAX_ALARMS 10
#define MAX_TIMERS 5

// Time structure
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

// Alarm structure
typedef struct {
    Time time;
    bool enabled;
    bool repeat;
    uint8_t days;  // Bit field for days of week
    char label[32];
    uint32_t next_trigger;
} Alarm;

// Timer structure
typedef struct {
    uint32_t duration;     // in seconds
    uint32_t remaining;    // in seconds
    bool running;
    char label[32];
    uint32_t start_time;
} Timer;

// App state
typedef struct {
    // Windows
    UiWindow* main_window;
    UiWindow* alarm_window;
    UiWindow* timer_window;
    UiWindow* stopwatch_window;
    
    // Clock elements
    UiLabel* time_label;
    UiLabel* date_label;
    UiLabel* day_label;
    
    // Alarm elements
    UiListBox* alarm_list;
    UiButton* add_alarm_button;
    UiButton* edit_alarm_button;
    
    // Timer elements
    UiListBox* timer_list;
    UiButton* add_timer_button;
    UiLabel* timer_display;
    UiButton* start_timer_button;
    UiButton* pause_timer_button;
    UiButton* reset_timer_button;
    
    // Stopwatch elements
    UiLabel* stopwatch_display;
    UiButton* start_stopwatch_button;
    UiButton* lap_button;
    UiListBox* lap_list;
    
    // Navigation
    UiButton* clock_tab;
    UiButton* alarm_tab;
    UiButton* timer_tab;
    UiButton* stopwatch_tab;
    
    // State
    Alarm alarms[MAX_ALARMS];
    size_t alarm_count;
    Timer timers[MAX_TIMERS];
    size_t timer_count;
    uint32_t stopwatch_start;
    uint32_t stopwatch_elapsed;
    bool stopwatch_running;
    uint32_t* lap_times;
    size_t lap_count;
    SecureStorage* settings_storage;
} ClockApp;

static ClockApp app_state;

// System time utilities
static uint32_t get_system_time(void) {
    uint64_t uptime_ms;
    if (hal_clock_get_uptime(&uptime_ms) != CLOCK_HAL_SUCCESS) {
        return 0;
    }
    return (uint32_t)(uptime_ms / 1000);
}

// Update clock display
static void update_clock_display(void) {
    struct tm current_time;
    if (hal_rtc_get_time(&current_time) != CLOCK_HAL_SUCCESS) {
        return;
    }
    
    char time_text[16];
    char date_text[32];
    char day_text[16];
    
    // Format time (12/24 hour based on settings)
    strftime(time_text, sizeof(time_text),
             "%H:%M:%S", &current_time);
    
    // Format date
    strftime(date_text, sizeof(date_text),
             "%B %d, %Y", &current_time);
    
    // Format day
    strftime(day_text, sizeof(day_text),
             "%A", &current_time);
    
    ui_set_text((UiElement*)app_state.time_label, time_text);
    ui_set_text((UiElement*)app_state.date_label, date_text);
    ui_set_text((UiElement*)app_state.day_label, day_text);
}

// Update stopwatch display
static void update_stopwatch_display(void) {
    if (!app_state.stopwatch_running) {
        return;
    }
    
    uint32_t current_time = get_system_time();
    uint32_t elapsed = current_time - app_state.stopwatch_start + app_state.stopwatch_elapsed;
    
    uint32_t hours = elapsed / 3600;
    uint32_t minutes = (elapsed % 3600) / 60;
    uint32_t seconds = elapsed % 60;
    uint32_t centiseconds = (elapsed % 100);
    
    char display[32];
    snprintf(display, sizeof(display),
             "%02u:%02u:%02u.%02u",
             hours, minutes, seconds, centiseconds);
    
    ui_set_text((UiElement*)app_state.stopwatch_display, display);
}

// Update timer displays
static void update_timer_displays(void) {
    for (size_t i = 0; i < app_state.timer_count; i++) {
        Timer* timer = &app_state.timers[i];
        if (timer->running) {
            uint32_t current_time = get_system_time();
            uint32_t elapsed = current_time - timer->start_time;
            if (elapsed >= timer->duration) {
                timer->running = false;
                timer->remaining = 0;
                
                char message[64];
                snprintf(message, sizeof(message),
                        "Timer finished: %s", timer->label[0] ? timer->label : "Untitled");
                trigger_notification("CerebroOS Clock", message);
            } else {
                timer->remaining = timer->duration - elapsed;
            }
        }
        
        // Update display
        uint32_t hours = timer->remaining / 3600;
        uint32_t minutes = (timer->remaining % 3600) / 60;
        uint32_t seconds = timer->remaining % 60;
        
        char display[32];
        snprintf(display, sizeof(display),
                 "%02u:%02u:%02u",
                 hours, minutes, seconds);
        
        // Update timer in list
        char list_item[64];
        snprintf(list_item, sizeof(list_item),
                 "%s: %s",
                 timer->label, display);
        ui_listbox_set_item(app_state.timer_list, i, list_item);
    }
}

// Check and trigger alarms
static void check_alarms(void) {
    struct tm current_time;
    if (hal_rtc_get_time(&current_time) != CLOCK_HAL_SUCCESS) {
        return;
    }
    
    for (size_t i = 0; i < app_state.alarm_count; i++) {
        Alarm* alarm = &app_state.alarms[i];
        if (!alarm->enabled) {
            continue;
        }
        
        RTCAlarm rtc_alarm = {
            .hour = alarm->time.hour,
            .minute = alarm->time.minute,
            .second = alarm->time.second,
            .day_mask = alarm->days,
            .enabled = true,
            .callback = NULL,  // We handle notifications ourselves
            .callback_data = NULL
        };
        
        if (hal_rtc_set_alarm(i, &rtc_alarm) != CLOCK_HAL_SUCCESS) {
            continue;
        }
        
        if (current_time.tm_hour == alarm->time.hour &&
            current_time.tm_min == alarm->time.minute &&
            current_time.tm_sec == alarm->time.second) {
            
            char message[64];
            snprintf(message, sizeof(message),
                    "Alarm: %s", alarm->label[0] ? alarm->label : "Untitled");
            
            trigger_notification("CerebroOS Clock", message);
            
            if (alarm->repeat) {
                // Calculate next trigger time using HAL
                alarm->next_trigger += 24 * 60 * 60;  // Add one day
                
                if (alarm->days) {
                    uint8_t current_day = current_time.tm_wday;
                    uint8_t next_day = current_day;
                    
                    do {
                        next_day = (next_day + 1) % 7;
                        if (alarm->days & (1 << next_day)) {
                            break;
                        }
                    } while (next_day != current_day);
                    
                    int days_until = (next_day + 7 - current_day) % 7;
                    alarm->next_trigger += days_until * 24 * 60 * 60;
                }
            } else {
                alarm->enabled = false;
                hal_rtc_enable_alarm(i, false);
            }
        }
    }
}

// Notification utilities
static void trigger_notification(const char* title, const char* message) {
    NotificationParams params = {
        .title = title,
        .message = message,
        .priority = NOTIFICATION_PRIORITY_HIGH,
        .persistent = true,
        .vibrate = true,
        .sound = "alarm.wav"
    };
    notification_send(&params);
}

// Timer callback for regular updates
static void clock_timer_callback(void* data) {
    update_clock_display();
    check_alarms();
    update_timer_displays();
    if (app_state.stopwatch_running) {
        update_stopwatch_display();
    }
    
    // Schedule next update
    TimerConfig timer_config = {
        .interval_ms = 1000,
        .repeat = true,
        .callback = clock_timer_callback,
        .callback_data = NULL
    };
    
    if (hal_timer_start(0, &timer_config) != CLOCK_HAL_SUCCESS) {
        // Handle error
        return;
    }
}

// Tab switching
static void hide_all_views(void) {
    ui_set_visible((UiElement*)app_state.time_label, false);
    ui_set_visible((UiElement*)app_state.date_label, false);
    ui_set_visible((UiElement*)app_state.day_label, false);
    
    ui_set_visible((UiElement*)app_state.alarm_list, false);
    ui_set_visible((UiElement*)app_state.add_alarm_button, false);
    ui_set_visible((UiElement*)app_state.edit_alarm_button, false);
    
    ui_set_visible((UiElement*)app_state.timer_list, false);
    ui_set_visible((UiElement*)app_state.add_timer_button, false);
    ui_set_visible((UiElement*)app_state.timer_display, false);
    ui_set_visible((UiElement*)app_state.start_timer_button, false);
    ui_set_visible((UiElement*)app_state.pause_timer_button, false);
    ui_set_visible((UiElement*)app_state.reset_timer_button, false);
    
    ui_set_visible((UiElement*)app_state.stopwatch_display, false);
    ui_set_visible((UiElement*)app_state.start_stopwatch_button, false);
    ui_set_visible((UiElement*)app_state.lap_button, false);
    ui_set_visible((UiElement*)app_state.lap_list, false);
}

static void show_clock_view(void) {
    hide_all_views();
    ui_set_visible((UiElement*)app_state.time_label, true);
    ui_set_visible((UiElement*)app_state.date_label, true);
    ui_set_visible((UiElement*)app_state.day_label, true);
    update_clock_display();
}

static void show_alarm_view(void) {
    hide_all_views();
    ui_set_visible((UiElement*)app_state.alarm_list, true);
    ui_set_visible((UiElement*)app_state.add_alarm_button, true);
    ui_set_visible((UiElement*)app_state.edit_alarm_button, true);
}

static void show_timer_view(void) {
    hide_all_views();
    ui_set_visible((UiElement*)app_state.timer_list, true);
    ui_set_visible((UiElement*)app_state.add_timer_button, true);
    ui_set_visible((UiElement*)app_state.timer_display, true);
    ui_set_visible((UiElement*)app_state.start_timer_button, true);
    ui_set_visible((UiElement*)app_state.pause_timer_button, true);
    ui_set_visible((UiElement*)app_state.reset_timer_button, true);
    update_timer_displays();
}

static void show_stopwatch_view(void) {
    hide_all_views();
    ui_set_visible((UiElement*)app_state.stopwatch_display, true);
    ui_set_visible((UiElement*)app_state.start_stopwatch_button, true);
    ui_set_visible((UiElement*)app_state.lap_button, true);
    ui_set_visible((UiElement*)app_state.lap_list, true);
    update_stopwatch_display();
}

// Tab button callbacks
static void on_clock_tab_click(UiElement* element) {
    show_clock_view();
}

static void on_alarm_tab_click(UiElement* element) {
    show_alarm_view();
}

static void on_timer_tab_click(UiElement* element) {
    show_timer_view();
}

static void on_stopwatch_tab_click(UiElement* element) {
    show_stopwatch_view();
}

// Button callbacks
static void on_start_stopwatch_click(UiElement* element) {
    if (!app_state.stopwatch_running) {
        uint32_t current_time = get_system_time();
        app_state.stopwatch_start = current_time;
        app_state.stopwatch_running = true;
        ui_set_text((UiElement*)app_state.start_stopwatch_button, "Stop");
    } else {
        uint32_t current_time = get_system_time();
        app_state.stopwatch_elapsed += current_time - app_state.stopwatch_start;
        app_state.stopwatch_running = false;
        ui_set_text((UiElement*)app_state.start_stopwatch_button, "Start");
    }
}

static void on_lap_click(UiElement* element) {
    if (!app_state.stopwatch_running) {
        return;
    }
    
    // Record lap time
    uint32_t current_time = get_system_time();
    uint32_t lap_time = current_time - app_state.stopwatch_start + app_state.stopwatch_elapsed;
    
    // Add to lap times array
    app_state.lap_times = realloc(app_state.lap_times,
                                 (app_state.lap_count + 1) * sizeof(uint32_t));
    app_state.lap_times[app_state.lap_count++] = lap_time;
    
    // Format and add to list
    char lap_text[32];
    uint32_t hours = lap_time / 3600;
    uint32_t minutes = (lap_time % 3600) / 60;
    uint32_t seconds = lap_time % 60;
    uint32_t centiseconds = (lap_time % 100);
    
    snprintf(lap_text, sizeof(lap_text),
             "Lap %zu: %02u:%02u:%02u.%02u",
             app_state.lap_count,
             hours, minutes, seconds, centiseconds);
    
    ui_listbox_add_item(app_state.lap_list, lap_text);
}

// Initialize UI
static void init_ui(void) {
    // Create main window with tabs
    app_state.main_window = ui_create_window(
        "Clock",
        0, 0, 240, 320
    );
    
    // Create tab buttons
    app_state.clock_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        0, 0, 60, 30,
        "Clock"
    );
    
    app_state.alarm_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        60, 0, 60, 30,
        "Alarm"
    );
    
    app_state.timer_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        120, 0, 60, 30,
        "Timer"
    );
    
    app_state.stopwatch_tab = ui_create_button(
        (UiElement*)app_state.main_window,
        180, 0, 60, 30,
        "SW"
    );
    
    // Create clock elements
    app_state.time_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 40, 230, 60
    );
    app_state.time_label->base.fg_color = UI_COLOR_BLUE;
    
    app_state.date_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 105, 230, 30
    );
    
    app_state.day_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 140, 230, 30
    );
    
    // Create stopwatch elements
    app_state.stopwatch_display = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 40, 230, 60
    );
    
    app_state.start_stopwatch_button = ui_create_button(
        (UiElement*)app_state.main_window,
        5, 105, 110, 40,
        "Start"
    );
    app_state.start_stopwatch_button->base.on_click = on_start_stopwatch_click;
    
    app_state.lap_button = ui_create_button(
        (UiElement*)app_state.main_window,
        125, 105, 110, 40,
        "Lap"
    );
    app_state.lap_button->base.on_click = on_lap_click;
    
    app_state.lap_list = ui_create_listbox(
        (UiElement*)app_state.main_window,
        5, 150, 230, 165
    );
    
    // Add tab button callbacks
    app_state.clock_tab->base.on_click = on_clock_tab_click;
    app_state.alarm_tab->base.on_click = on_alarm_tab_click;
    app_state.timer_tab->base.on_click = on_timer_tab_click;
    app_state.stopwatch_tab->base.on_click = on_stopwatch_tab_click;
    
    // Show clock view by default
    show_clock_view();
}

// Initialize storage
static void init_storage(void) {
    app_state.settings_storage = secure_storage_create("clock_settings", 
        sizeof(size_t) + (MAX_ALARMS * sizeof(Alarm)));
    
    if (app_state.settings_storage) {
        // Load alarms
        secure_storage_read(app_state.settings_storage,
                          &app_state.alarm_count,
                          sizeof(size_t));
        
        secure_storage_read(app_state.settings_storage,
                          app_state.alarms,
                          app_state.alarm_count * sizeof(Alarm));
    }
}

// App lifecycle callbacks
static void on_create(void) {
    memset(&app_state, 0, sizeof(ClockApp));
    
    // Initialize HAL
    RTCConfig rtc_config = {
        .hour = 0,
        .minute = 0,
        .second = 0,
        .day = 1,
        .month = 1,
        .year = 2024,
        .twenty_four_hour = true,
        .timezone_offset = 0
    };
    
    if (hal_rtc_init(&rtc_config) != CLOCK_HAL_SUCCESS) {
        // Handle error
        return;
    }
    
    if (hal_timer_init() != CLOCK_HAL_SUCCESS) {
        // Handle error
        return;
    }
    
    // Initialize UI and storage
    init_ui();
    init_storage();
    
    // Configure and start update timer
    TimerConfig timer_config = {
        .interval_ms = 1000,
        .repeat = true,
        .callback = clock_timer_callback,
        .callback_data = NULL
    };
    
    if (hal_timer_start(0, &timer_config) != CLOCK_HAL_SUCCESS) {
        // Handle error
        return;
    }
    
    // Enable power-saving features
    hal_clock_set_power_mode(CLOCK_POWER_SAVING);
    hal_clock_enable_wakeup(true);
}

static void on_destroy(void) {
    // Stop timer
    hal_timer_stop(0);
    
    // Disable all alarms
    for (size_t i = 0; i < MAX_HW_ALARMS; i++) {
        hal_rtc_enable_alarm(i, false);
    }
    
    // Save settings
    if (app_state.settings_storage) {
        secure_storage_write(app_state.settings_storage,
                           &app_state.alarm_count,
                           sizeof(size_t));
        
        secure_storage_write(app_state.settings_storage,
                           app_state.alarms,
                           app_state.alarm_count * sizeof(Alarm));
        
        secure_storage_destroy(app_state.settings_storage);
    }
    
    if (app_state.lap_times) {
        free(app_state.lap_times);
    }
}

static void on_pause(void) {
    // Save any state if needed
}

static void on_resume(void) {
    // Refresh displays
    update_clock_display();
    update_stopwatch_display();
    update_timer_displays();
}

// Register app
APP_DECLARE("clock_app") {
    .on_create = on_create,
    .on_destroy = on_destroy,
    .on_pause = on_pause,
    .on_resume = on_resume,
    .required_permissions = 0  // No special permissions needed
};
