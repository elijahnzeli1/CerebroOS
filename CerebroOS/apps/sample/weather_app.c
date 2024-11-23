#include "../../os/app_framework.h"
#include "../../os/ui_framework.h"
#include "../../os/network.h"
#include "../../os/security.h"
#include <string.h>
#include <stdio.h>

// Weather data structure
typedef struct {
    char location[64];
    char condition[32];
    int temperature;
    int humidity;
    int wind_speed;
    uint32_t last_update;
} WeatherData;

// App state
typedef struct {
    UiWindow* main_window;
    UiLabel* location_label;
    UiLabel* temp_label;
    UiLabel* condition_label;
    UiLabel* humidity_label;
    UiLabel* wind_label;
    UiButton* refresh_button;
    WeatherData current_weather;
    SecureStorage* weather_cache;
    uint32_t update_interval;  // in seconds
} WeatherApp;

static WeatherApp app_state;

// Format weather data into UI labels
static void update_weather_display(void) {
    char buffer[64];
    
    // Update location
    ui_set_text((UiElement*)app_state.location_label, app_state.current_weather.location);
    
    // Update temperature
    snprintf(buffer, sizeof(buffer), "%d°C", app_state.current_weather.temperature);
    ui_set_text((UiElement*)app_state.temp_label, buffer);
    
    // Update condition
    ui_set_text((UiElement*)app_state.condition_label, app_state.current_weather.condition);
    
    // Update humidity
    snprintf(buffer, sizeof(buffer), "Humidity: %d%%", app_state.current_weather.humidity);
    ui_set_text((UiElement*)app_state.humidity_label, buffer);
    
    // Update wind
    snprintf(buffer, sizeof(buffer), "Wind: %d km/h", app_state.current_weather.wind_speed);
    ui_set_text((UiElement*)app_state.wind_label, buffer);
    
    // Invalidate window to trigger redraw
    ui_invalidate((UiElement*)app_state.main_window);
}

// Fetch weather data from network
static bool fetch_weather_data(void) {
    // Check network status
    NetworkInfo net_info = network_get_info();
    if (net_info.status != NETWORK_STATUS_CONNECTED) {
        return false;
    }
    
    // Create HTTP request
    HttpRequest request = {
        .method = "GET",
        .timeout_ms = 5000
    };
    snprintf(request.url, sizeof(request.url), 
             "https://api.weather.example.com/current?location=%s",
             app_state.current_weather.location);
    
    // Send request
    HttpResponse response;
    if (!http_send_request(&request, &response)) {
        return false;
    }
    
    if (response.status_code == 200 && response.body) {
        // Parse JSON response and update weather data
        // (Implementation omitted for brevity)
        
        // Update last update timestamp
        app_state.current_weather.last_update = /* Get current time */;
        
        // Cache the weather data
        secure_storage_write(app_state.weather_cache, 
                           &app_state.current_weather, 
                           sizeof(WeatherData));
        
        // Update display
        update_weather_display();
        
        http_free_response(&response);
        return true;
    }
    
    http_free_response(&response);
    return false;
}

// Refresh button callback
static void on_refresh_click(UiElement* element) {
    // Check network permission
    if (!security_has_permission("weather_app", PERM_NETWORK_ACCESS)) {
        if (!security_request_permission("weather_app", PERM_NETWORK_ACCESS)) {
            return;
        }
    }
    
    // Set to low power mode while fetching
    network_set_power_mode(true);
    
    // Fetch and update
    fetch_weather_data();
    
    // Restore normal power mode
    network_set_power_mode(false);
}

// Initialize UI
static void init_ui(void) {
    // Create main window
    app_state.main_window = ui_create_window(
        "Weather",
        0, 0, 240, 320
    );
    
    // Create location label
    app_state.location_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 5, 230, 30
    );
    
    // Create temperature label
    app_state.temp_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 40, 230, 50
    );
    app_state.temp_label->base.fg_color = UI_COLOR_BLUE;
    
    // Create condition label
    app_state.condition_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 95, 230, 30
    );
    
    // Create humidity label
    app_state.humidity_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 130, 230, 30
    );
    
    // Create wind label
    app_state.wind_label = ui_create_label(
        (UiElement*)app_state.main_window,
        5, 165, 230, 30
    );
    
    // Create refresh button
    app_state.refresh_button = ui_create_button(
        (UiElement*)app_state.main_window,
        85, 250, 70, 30,
        "Refresh"
    );
    app_state.refresh_button->base.on_click = on_refresh_click;
}

// Initialize storage
static void init_storage(void) {
    app_state.weather_cache = secure_storage_create("weather_cache", sizeof(WeatherData));
    if (app_state.weather_cache) {
        // Try to load cached data
        WeatherData cached_data;
        if (secure_storage_read(app_state.weather_cache, &cached_data, sizeof(WeatherData))) {
            memcpy(&app_state.current_weather, &cached_data, sizeof(WeatherData));
            update_weather_display();
        }
    }
}

// App lifecycle callbacks
static void on_create(void) {
    memset(&app_state, 0, sizeof(WeatherApp));
    
    // Set default values
    strncpy(app_state.current_weather.location, "London", sizeof(app_state.current_weather.location) - 1);
    app_state.update_interval = 1800;  // 30 minutes
    
    // Initialize components
    init_ui();
    init_storage();
    
    // Request permissions
    security_request_permission("weather_app", PERM_NETWORK_ACCESS);
    security_request_permission("weather_app", PERM_LOCATION_ACCESS);
    
    // Initial weather fetch
    fetch_weather_data();
}

static void on_destroy(void) {
    if (app_state.weather_cache) {
        secure_storage_destroy(app_state.weather_cache);
    }
}

static void on_pause(void) {
    // Save any state if needed
}

static void on_resume(void) {
    // Check if we need to update
    uint32_t current_time = /* Get current time */;
    if (current_time - app_state.current_weather.last_update > app_state.update_interval) {
        fetch_weather_data();
    }
}

// Register app
APP_DECLARE("weather_app") {
    .on_create = on_create,
    .on_destroy = on_destroy,
    .on_pause = on_pause,
    .on_resume = on_resume,
    .required_permissions = PERM_NETWORK_ACCESS | PERM_LOCATION_ACCESS
};
