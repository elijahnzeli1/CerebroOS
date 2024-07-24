#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h> // For standard integer types (e.g., uint32_t)

// --- Kernel Event Types (for improved modularity) ---

// Define an enum for different types of kernel events
typedef enum {
    KERNEL_EVENT_NONE,
    KERNEL_EVENT_UI_UPDATE,    // Trigger UI redraw
    KERNEL_EVENT_POWER_LOW,    // Low battery warning
    KERNEL_EVENT_TIMER_TICK,   // Periodic timer tick
    // ... add more events as needed (e.g., network, sensor events)
} KernelEvent;

// --- Function Declarations ---

// Initialize the kernel (memory, processes, UI, etc.)
void kernel_init();

// Main kernel loop (runs continuously)
void kernel_main();

// Trigger a kernel event 
void kernel_trigger_event(KernelEvent event); 

// Optional Functions (add as needed)

// Get the current kernel time in milliseconds
uint32_t kernel_get_time();

// Sleep for a specified number of milliseconds (non-blocking)
void kernel_sleep(uint32_t milliseconds);


#endif // KERNEL_H
