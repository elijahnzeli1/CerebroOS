#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include <stdint.h> // For standard integer types

// Define an enum for power modes to improve readability and maintainability
typedef enum {
    POWER_MODE_NORMAL,
    POWER_MODE_LOW_POWER,
    POWER_MODE_CRITICAL_POWER,
    // Add more power modes as needed (e.g., PERFORMANCE, CUSTOM)
} PowerMode;

// Define a struct for holding power-related information
typedef struct {
    int batteryLevel; // Current battery level (percentage)
    PowerMode mode;   // Current power mode
    // ... additional fields for temperature, charging status, etc.
} PowerStatus;

// Function declarations

// Initialize power management system
void power_init();

// Get the current power status
PowerStatus power_get_status();

// Set the power mode (e.g., normal, low power, etc.)
void power_set_mode(PowerMode mode);

// Periodically called function to manage power based on current conditions
void power_manage();

// Additional functions for advanced power management (optional)
void power_enable_feature(const char* featureName);
void power_disable_feature(const char* featureName);

#endif // POWER_MANAGEMENT_H
