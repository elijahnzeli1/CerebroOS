#ifndef CEREBRO_OS_EMULATOR_H
#define CEREBRO_OS_EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

// Emulator Configuration
typedef struct {
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t memory_size;
    uint32_t cpu_frequency;
    bool enable_network;
    bool enable_power_simulation;
    const char* storage_dir;
} EmulatorConfig;

// Virtual Hardware Components
typedef struct {
    void* display_buffer;
    void* input_state;
    void* memory_map;
    void* network_interface;
    void* power_controller;
    void* rtc;
} VirtualHardware;

// Emulator Statistics
typedef struct {
    uint32_t cpu_usage;
    uint32_t memory_usage;
    uint32_t power_consumption;
    uint32_t network_traffic;
    double fps;
    uint32_t input_events;
} EmulatorStats;

// Emulator Control Functions
bool emulator_init(const EmulatorConfig* config);
void emulator_shutdown(void);
void emulator_pause(void);
void emulator_resume(void);
void emulator_reset(void);

// Virtual Hardware Access
VirtualHardware* emulator_get_hardware(void);
void emulator_update_display(const void* buffer, uint32_t size);
void emulator_process_input(void);
void emulator_simulate_network(void);
void emulator_update_power_state(void);

// Monitoring and Debug
EmulatorStats emulator_get_stats(void);
void emulator_dump_memory(const char* filename);
void emulator_log_event(const char* event);
void emulator_set_breakpoint(uint32_t address);
void emulator_single_step(void);

// Testing Utilities
void emulator_inject_input(const void* input_data, uint32_t size);
void emulator_simulate_low_battery(void);
void emulator_simulate_network_error(void);
void emulator_simulate_memory_pressure(void);
void emulator_simulate_cpu_load(uint32_t percentage);

#endif // CEREBRO_OS_EMULATOR_H
