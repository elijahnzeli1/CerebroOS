#ifndef APP_FRAMEWORK_H
#define APP_FRAMEWORK_H

#include <stdint.h>
#include <stdbool.h>

// App lifecycle states
typedef enum {
    APP_STATE_CREATED,
    APP_STATE_INITIALIZED,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_STOPPED,
    APP_STATE_DESTROYED
} AppState;

// Resource usage limits
typedef struct {
    uint32_t max_memory_kb;
    uint32_t max_cpu_percent;
    uint32_t max_storage_kb;
    bool background_allowed;
    bool network_access;
} AppResourceLimits;

// App configuration
typedef struct {
    char name[32];
    char version[16];
    char developer[64];
    AppResourceLimits resource_limits;
    void (*on_create)(void);
    void (*on_start)(void);
    void (*on_pause)(void);
    void (*on_resume)(void);
    void (*on_stop)(void);
    void (*on_destroy)(void);
} AppConfig;

// App instance
typedef struct {
    AppConfig config;
    AppState state;
    uint32_t memory_usage;
    uint32_t cpu_usage;
    uint32_t storage_usage;
    void* app_data;
} AppInstance;

// App Framework API
bool app_register(AppConfig* config);
bool app_start(const char* app_name);
bool app_pause(const char* app_name);
bool app_resume(const char* app_name);
bool app_stop(const char* app_name);
bool app_unregister(const char* app_name);

// Resource Management API
void* app_allocate_memory(const char* app_name, size_t size);
bool app_free_memory(const char* app_name, void* ptr);
bool app_request_storage(const char* app_name, size_t size);
bool app_release_storage(const char* app_name, size_t size);

// Event System
typedef void (*EventHandler)(void* data);
bool app_register_event_handler(const char* app_name, const char* event_name, EventHandler handler);
bool app_unregister_event_handler(const char* app_name, const char* event_name);
bool app_emit_event(const char* event_name, void* data);

#endif // APP_FRAMEWORK_H
