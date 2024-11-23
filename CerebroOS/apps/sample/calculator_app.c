#include "../../os/app_framework.h"
#include <stdio.h>
#include <string.h>

// App-specific data structure
typedef struct {
    int last_result;
    char operation_history[256];
} CalculatorData;

// Event handlers
static void on_number_pressed(void* data) {
    int number = *(int*)data;
    AppInstance* app = find_app("Calculator");
    CalculatorData* calc_data = (CalculatorData*)app->app_data;
    
    // Add number to history
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", number);
    strcat(calc_data->operation_history, buf);
}

static void on_operation_pressed(void* data) {
    char operation = *(char*)data;
    AppInstance* app = find_app("Calculator");
    CalculatorData* calc_data = (CalculatorData*)app->app_data;
    
    // Add operation to history
    char buf[2] = {operation, '\0'};
    strcat(calc_data->operation_history, buf);
}

// Lifecycle callbacks
static void calculator_create(void) {
    AppInstance* app = find_app("Calculator");
    
    // Allocate app-specific data
    app->app_data = app_allocate_memory("Calculator", sizeof(CalculatorData));
    CalculatorData* calc_data = (CalculatorData*)app->app_data;
    calc_data->last_result = 0;
    calc_data->operation_history[0] = '\0';
    
    // Register event handlers
    app_register_event_handler("Calculator", "number_pressed", on_number_pressed);
    app_register_event_handler("Calculator", "operation_pressed", on_operation_pressed);
}

static void calculator_start(void) {
    // Initialize UI and show calculator
    printf("Calculator started\n");
}

static void calculator_pause(void) {
    // Save state if needed
    printf("Calculator paused\n");
}

static void calculator_resume(void) {
    // Restore state if needed
    printf("Calculator resumed\n");
}

static void calculator_stop(void) {
    // Clean up UI
    printf("Calculator stopped\n");
}

static void calculator_destroy(void) {
    AppInstance* app = find_app("Calculator");
    
    // Free app-specific data
    if (app->app_data) {
        app_free_memory("Calculator", app->app_data);
        app->app_data = NULL;
    }
    
    // Unregister event handlers
    app_unregister_event_handler("Calculator", "number_pressed");
    app_unregister_event_handler("Calculator", "operation_pressed");
}

// Register calculator app
void register_calculator_app(void) {
    AppConfig config = {
        .name = "Calculator",
        .version = "1.0.0",
        .developer = "CerebroOS Team",
        .resource_limits = {
            .max_memory_kb = 64,      // 64KB max memory
            .max_cpu_percent = 10,     // 10% CPU usage limit
            .max_storage_kb = 128,     // 128KB storage limit
            .background_allowed = false,
            .network_access = false
        },
        .on_create = calculator_create,
        .on_start = calculator_start,
        .on_pause = calculator_pause,
        .on_resume = calculator_resume,
        .on_stop = calculator_stop,
        .on_destroy = calculator_destroy
    };
    
    app_register(&config);
}
