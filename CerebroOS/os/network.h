#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

// Network types
typedef enum {
    NETWORK_TYPE_NONE,
    NETWORK_TYPE_GSM,
    NETWORK_TYPE_GPRS,
    NETWORK_TYPE_EDGE,
    NETWORK_TYPE_3G,
    NETWORK_TYPE_4G,
    NETWORK_TYPE_WIFI
} NetworkType;

// Network status
typedef enum {
    NETWORK_STATUS_DISCONNECTED,
    NETWORK_STATUS_CONNECTING,
    NETWORK_STATUS_CONNECTED,
    NETWORK_STATUS_ERROR
} NetworkStatus;

// Signal strength
typedef struct {
    int8_t rssi;          // Received Signal Strength Indicator (dBm)
    uint8_t quality;      // Signal quality (0-100%)
    uint8_t bars;         // Signal bars (0-5)
} SignalStrength;

// Network configuration
typedef struct {
    NetworkType preferred_type;
    bool auto_connect;
    bool roaming_enabled;
    uint16_t connect_timeout_ms;
    uint16_t retry_interval_ms;
    uint8_t max_retries;
} NetworkConfig;

// Network statistics
typedef struct {
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t errors;
    uint32_t connection_time;
} NetworkStats;

// Network info
typedef struct {
    NetworkType type;
    NetworkStatus status;
    SignalStrength signal;
    char operator_name[32];
    char network_name[32];
    bool roaming;
    uint8_t battery_impact;    // Estimated battery impact (0-100%)
} NetworkInfo;

// Socket types
typedef enum {
    SOCKET_TYPE_TCP,
    SOCKET_TYPE_UDP
} SocketType;

// Socket status
typedef enum {
    SOCKET_STATUS_CLOSED,
    SOCKET_STATUS_LISTENING,
    SOCKET_STATUS_CONNECTING,
    SOCKET_STATUS_CONNECTED,
    SOCKET_STATUS_ERROR
} SocketStatus;

// Socket handle
typedef struct Socket Socket;

// Network callbacks
typedef void (*NetworkStatusCallback)(NetworkStatus status, void* user_data);
typedef void (*NetworkDataCallback)(const uint8_t* data, size_t size, void* user_data);
typedef void (*SocketEventCallback)(Socket* socket, SocketStatus status, void* user_data);

// Network initialization and configuration
bool network_init(const NetworkConfig* config);
void network_shutdown(void);
void network_set_config(const NetworkConfig* config);
NetworkConfig network_get_config(void);

// Network status and control
NetworkStatus network_get_status(void);
NetworkInfo network_get_info(void);
NetworkStats network_get_stats(void);
bool network_connect(void);
void network_disconnect(void);
void network_set_status_callback(NetworkStatusCallback callback, void* user_data);

// Power management
void network_set_power_mode(bool low_power);
uint8_t network_get_power_usage(void);

// Socket operations
Socket* socket_create(SocketType type);
void socket_destroy(Socket* socket);
bool socket_connect(Socket* socket, const char* host, uint16_t port);
bool socket_listen(Socket* socket, uint16_t port);
bool socket_accept(Socket* socket, Socket* client_socket);
bool socket_send(Socket* socket, const void* data, size_t size);
bool socket_receive(Socket* socket, void* buffer, size_t size, size_t* bytes_received);
void socket_close(Socket* socket);
SocketStatus socket_get_status(const Socket* socket);
void socket_set_event_callback(Socket* socket, SocketEventCallback callback, void* user_data);

// HTTP client (simplified for feature phones)
typedef struct {
    char url[256];
    char method[8];
    char* headers;
    void* body;
    size_t body_size;
    uint16_t timeout_ms;
} HttpRequest;

typedef struct {
    uint16_t status_code;
    char* headers;
    void* body;
    size_t body_size;
} HttpResponse;

bool http_send_request(const HttpRequest* request, HttpResponse* response);
void http_free_response(HttpResponse* response);

// SMS functionality
typedef struct {
    char recipient[16];
    char message[160];
    bool flash_message;
    uint8_t priority;
} SmsMessage;

bool sms_send(const SmsMessage* message);
bool sms_is_available(void);
uint16_t sms_get_storage_capacity(void);
uint16_t sms_get_message_count(void);

#endif // NETWORK_H
