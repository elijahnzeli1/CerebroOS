#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <stdint.h> // For standard integer types
#include <stdbool.h>

// Expanded Network Types
typedef enum {
    NETWORK_NONE = 0,         // No network available
    NETWORK_WIFI,            
    NETWORK_CELLULAR,
    NETWORK_BLUETOOTH,        // Add Bluetooth support
    // ... add more network types as needed
} NetworkType;

// Error Codes for Network Operations
typedef enum {
    NETWORK_ERROR_NONE = 0,         // No error
    NETWORK_ERROR_HARDWARE,         // Hardware initialization or communication error
    NETWORK_ERROR_CONNECTION_FAILED, // Failed to establish a connection
    NETWORK_ERROR_NOT_CONNECTED,   // Operation attempted when not connected
    NETWORK_ERROR_ALREADY_CONNECTED, // Connection attempt when already connected
    NETWORK_ERROR_SEND_FAILED,      // Failed to send data
    NETWORK_ERROR_RECEIVE_FAILED,   // Failed to receive data
    NETWORK_ERROR_INVALID_TYPE      // Invalid network type
    // ... add more error codes as needed
} NetworkError;

// Network Configuration Structure
typedef struct {
    NetworkType type;        // Network type
    // ... other configuration parameters for each network type
    // (e.g., SSID, password for WiFi)
} NetworkConfig;

// Network Information Structure
typedef struct {
    NetworkType type;        // Current network type
    bool connected;           // Connection status
    int signalStrength;     // Signal strength (if applicable)
    // ... other network information (e.g., IP address, MAC address)
} NetworkInfo;


// Function Declarations

// Initialize the network manager
NetworkError network_init();

// Connect to a network with the specified configuration
NetworkError network_connect(const NetworkConfig* config);

// Disconnect from the current network
NetworkError network_disconnect();

// Check if connected to a network
bool network_is_connected();

// Get information about the current network
NetworkInfo network_get_info();

// Send data over the network
NetworkError network_send_data(const uint8_t* data, int length);

// Receive data from the network
NetworkError network_receive_data(uint8_t* buffer, int max_length, int* received_length);

// Optional Functions:
// - network_scan(): Scan for available networks
// - network_get_available_networks(): Get a list of available networks
// - network_set_power_mode(PowerMode mode): Adjust network power consumption

#endif // NETWORK_MANAGER_H
