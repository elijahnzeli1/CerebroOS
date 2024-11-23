#include "app_framework.h"
#include "../kernel/memory_manager.h"
#include <string.h>
#include <stdlib.h>

#define MAX_APPS 32
#define MAX_EVENTS 64
#define MAX_HANDLERS_PER_EVENT 8

typedef struct {
    char name[32];
    EventHandler handlers[MAX_HANDLERS_PER_EVENT];
    uint8_t handler_count;
} Event;

static struct {
    AppInstance apps[MAX_APPS];
    uint8_t app_count;
    Event events[MAX_EVENTS];
    uint8_t event_count;
} framework;

static AppInstance* find_app(const char* app_name) {
    for (int i = 0; i < framework.app_count; i++) {
        if (strcmp(framework.apps[i].config.name, app_name) == 0) {
            return &framework.apps[i];
        }
    }
    return NULL;
}

static Event* find_event(const char* event_name) {
    for (int i = 0; i < framework.event_count; i++) {
        if (strcmp(framework.events[i].name, event_name) == 0) {
            return &framework.events[i];
        }
    }
    return NULL;
}

bool app_register(AppConfig* config) {
    if (framework.app_count >= MAX_APPS) return false;
    if (find_app(config->name)) return false;
    
    AppInstance* instance = &framework.apps[framework.app_count++];
    memcpy(&instance->config, config, sizeof(AppConfig));
    instance->state = APP_STATE_CREATED;
    instance->memory_usage = 0;
    instance->cpu_usage = 0;
    instance->storage_usage = 0;
    instance->app_data = NULL;
    
    if (instance->config.on_create) {
        instance->config.on_create();
    }
    
    return true;
}

bool app_start(const char* app_name) {
    AppInstance* app = find_app(app_name);
    if (!app || app->state != APP_STATE_CREATED) return false;
    
    if (app->config.on_start) {
        app->config.on_start();
    }
    
    app->state = APP_STATE_RUNNING;
    return true;
}

bool app_pause(const char* app_name) {
    AppInstance* app = find_app(app_name);
    if (!app || app->state != APP_STATE_RUNNING) return false;
    
    if (app->config.on_pause) {
        app->config.on_pause();
    }
    
    app->state = APP_STATE_PAUSED;
    return true;
}

bool app_resume(const char* app_name) {
    AppInstance* app = find_app(app_name);
    if (!app || app->state != APP_STATE_PAUSED) return false;
    
    if (app->config.on_resume) {
        app->config.on_resume();
    }
    
    app->state = APP_STATE_RUNNING;
    return true;
}

bool app_stop(const char* app_name) {
    AppInstance* app = find_app(app_name);
    if (!app || (app->state != APP_STATE_RUNNING && app->state != APP_STATE_PAUSED)) return false;
    
    if (app->config.on_stop) {
        app->config.on_stop();
    }
    
    app->state = APP_STATE_STOPPED;
    return true;
}

bool app_unregister(const char* app_name) {
    AppInstance* app = find_app(app_name);
    if (!app) return false;
    
    if (app->config.on_destroy) {
        app->config.on_destroy();
    }
    
    // Free all resources
    if (app->app_data) {
        app_free_memory(app_name, app->app_data);
    }
    
    // Remove app from array
    int index = app - framework.apps;
    if (index < framework.app_count - 1) {
        memmove(&framework.apps[index], &framework.apps[index + 1], 
                (framework.app_count - index - 1) * sizeof(AppInstance));
    }
    framework.app_count--;
    
    return true;
}

void* app_allocate_memory(const char* app_name, size_t size) {
    AppInstance* app = find_app(app_name);
    if (!app) return NULL;
    
    // Check if allocation would exceed limits
    if (app->memory_usage + size > app->config.resource_limits.max_memory_kb * 1024) {
        return NULL;
    }
    
    void* ptr = memory_allocate(size);
    if (ptr) {
        app->memory_usage += size;
    }
    return ptr;
}

bool app_free_memory(const char* app_name, void* ptr) {
    AppInstance* app = find_app(app_name);
    if (!app || !ptr) return false;
    
    // Note: In a real implementation, we'd need to track the size of each allocation
    // to properly update memory_usage
    memory_free(ptr);
    return true;
}

bool app_request_storage(const char* app_name, size_t size) {
    AppInstance* app = find_app(app_name);
    if (!app) return false;
    
    if (app->storage_usage + size > app->config.resource_limits.max_storage_kb * 1024) {
        return false;
    }
    
    app->storage_usage += size;
    return true;
}

bool app_release_storage(const char* app_name, size_t size) {
    AppInstance* app = find_app(app_name);
    if (!app || app->storage_usage < size) return false;
    
    app->storage_usage -= size;
    return true;
}

bool app_register_event_handler(const char* app_name, const char* event_name, EventHandler handler) {
    if (!find_app(app_name)) return false;
    
    Event* event = find_event(event_name);
    if (!event) {
        if (framework.event_count >= MAX_EVENTS) return false;
        event = &framework.events[framework.event_count++];
        strncpy(event->name, event_name, sizeof(event->name) - 1);
        event->handler_count = 0;
    }
    
    if (event->handler_count >= MAX_HANDLERS_PER_EVENT) return false;
    
    event->handlers[event->handler_count++] = handler;
    return true;
}

bool app_unregister_event_handler(const char* app_name, const char* event_name) {
    if (!find_app(app_name)) return false;
    
    Event* event = find_event(event_name);
    if (!event) return false;
    
    // Remove all handlers for this app (in a real implementation, we'd need to track
    // which handler belongs to which app)
    event->handler_count = 0;
    return true;
}

bool app_emit_event(const char* event_name, void* data) {
    Event* event = find_event(event_name);
    if (!event) return false;
    
    for (int i = 0; i < event->handler_count; i++) {
        event->handlers[i](data);
    }
    
    return true;
}
