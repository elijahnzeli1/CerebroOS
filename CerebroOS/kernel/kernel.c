#include "kernel.h"
#include "memory_manager.h"
#include "process_manager.h"
#include "drivers/display_driver.h"
#include "ui/ui_manager.h"
#include "power_management.h" 
#include "error_handler.h"  // Optional, for logging errors
#include <stdio.h>

void kernel_init() {
    // Early Initialization
    display_init(); // Initialize display driver early for boot messages
    
    // Initialize Subsystems
    memory_init();
    process_init();
    ui_init();
    power_init(); // Initialize power management (if available)

    // Display Initial Message (e.g., boot logo)
    display_draw_text(10, 10, "CerebroOS Booting...", 0x0000); // Assuming black text
    display_update();
}

void kernel_main() {
    // Main Kernel Loop
    while (1) {
        // Process Scheduling
        schedule_processes();

        // UI Management
        ui_draw();     // Update the UI
        ui_handle_input(); // Process user input (touchscreen, buttons)

        // Power Management
        power_manage(); // Call periodically to adjust power settings

        // Optional: Kernel-level Logging
        if (error_occurred()) {
            // Log error details to a file or the display (if available)
        }

        // Yield CPU to avoid busy waiting (if possible on your platform)
        // yield_cpu();
    }
}
