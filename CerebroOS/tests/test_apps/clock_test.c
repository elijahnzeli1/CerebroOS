#include "../emulator/emulator.h"
#include "../../apps/sample/clock_app.h"
#include <stdio.h>

int main() {
    // Configure emulator
    EmulatorConfig config = {
        .screen_width = 320,
        .screen_height = 240,
        .memory_size = 1024 * 1024, // 1MB
        .cpu_frequency = 100000000,  // 100MHz
        .enable_network = true,
        .enable_power_simulation = true,
        .storage_dir = "storage"
    };

    // Initialize emulator
    if (!emulator_init(&config)) {
        printf("Failed to initialize emulator\n");
        return 1;
    }

    // Get virtual hardware
    VirtualHardware* hw = emulator_get_hardware();

    // Main emulation loop
    while (1) {
        // Process input events
        emulator_process_input();

        // Update clock display
        clock_app_update();

        // Update emulator display
        emulator_update_display(hw->display_buffer, 
                              config.screen_width * config.screen_height * 4);

        // Get and print emulator statistics
        EmulatorStats stats = emulator_get_stats();
        printf("FPS: %.2f, CPU: %d%%, Memory: %d bytes\n",
               stats.fps, stats.cpu_usage, stats.memory_usage);

        // Test various scenarios
        static int test_phase = 0;
        if (stats.fps > 0 && test_phase == 0) {
            printf("Testing low battery scenario...\n");
            emulator_simulate_low_battery();
            test_phase++;
        } else if (test_phase == 1) {
            printf("Testing memory pressure...\n");
            emulator_simulate_memory_pressure();
            test_phase++;
        } else if (test_phase == 2) {
            printf("Testing network error...\n");
            emulator_simulate_network_error();
            test_phase++;
        }

        // Break if emulator is no longer running
        if (!emu_state.running) break;
    }

    // Cleanup
    emulator_shutdown();
    return 0;
}
