#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>
#include "app_framework.h"

// Permission types
typedef enum {
    PERM_NONE              = 0,
    PERM_STORAGE_READ      = 1 << 0,
    PERM_STORAGE_WRITE     = 1 << 1,
    PERM_NETWORK_ACCESS    = 1 << 2,
    PERM_SMS_SEND          = 1 << 3,
    PERM_SMS_RECEIVE       = 1 << 4,
    PERM_LOCATION_ACCESS   = 1 << 5,
    PERM_CAMERA_ACCESS     = 1 << 6,
    PERM_CONTACTS_READ     = 1 << 7,
    PERM_CONTACTS_WRITE    = 1 << 8,
    PERM_CALENDAR_READ     = 1 << 9,
    PERM_CALENDAR_WRITE    = 1 << 10,
    PERM_BACKGROUND_RUN    = 1 << 11,
    PERM_SYSTEM_SETTINGS   = 1 << 12,
    PERM_ADMIN            = 1 << 15
} Permission;

// Security levels
typedef enum {
    SECURITY_LEVEL_LOW,
    SECURITY_LEVEL_MEDIUM,
    SECURITY_LEVEL_HIGH,
    SECURITY_LEVEL_CUSTOM
} SecurityLevel;

// App signature verification status
typedef enum {
    SIGNATURE_VALID,
    SIGNATURE_INVALID,
    SIGNATURE_EXPIRED,
    SIGNATURE_UNKNOWN
} SignatureStatus;

// Security configuration
typedef struct {
    SecurityLevel level;
    uint32_t allowed_permissions;
    bool require_signatures;
    bool allow_unknown_sources;
    bool encrypt_storage;
    bool secure_boot;
} SecurityConfig;

// App security info
typedef struct {
    char app_id[64];
    uint32_t granted_permissions;
    SignatureStatus signature_status;
    char signature_issuer[64];
    uint64_t signature_expiry;
    bool is_system_app;
} AppSecurityInfo;

// Secure storage
typedef struct {
    char key[32];
    void* data;
    size_t size;
    bool encrypted;
    uint32_t access_permissions;
} SecureStorage;

// Security event types
typedef enum {
    SECURITY_EVENT_PERMISSION_GRANT,
    SECURITY_EVENT_PERMISSION_DENY,
    SECURITY_EVENT_AUTH_SUCCESS,
    SECURITY_EVENT_AUTH_FAIL,
    SECURITY_EVENT_SIGNATURE_INVALID,
    SECURITY_EVENT_POLICY_VIOLATION
} SecurityEventType;

// Security event data
typedef struct {
    SecurityEventType type;
    char app_id[64];
    uint32_t timestamp;
    char details[256];
} SecurityEvent;

// Security callbacks
typedef void (*SecurityEventCallback)(const SecurityEvent* event, void* user_data);
typedef bool (*PermissionRequestCallback)(const char* app_id, Permission permission, void* user_data);

// Security initialization and configuration
bool security_init(const SecurityConfig* config);
void security_shutdown(void);
void security_set_config(const SecurityConfig* config);
SecurityConfig security_get_config(void);

// Permission management
bool security_request_permission(const char* app_id, Permission permission);
bool security_has_permission(const char* app_id, Permission permission);
bool security_grant_permission(const char* app_id, Permission permission);
bool security_revoke_permission(const char* app_id, Permission permission);
void security_set_permission_callback(PermissionRequestCallback callback, void* user_data);

// App security
bool security_register_app(const char* app_id, const uint8_t* signature, size_t signature_size);
bool security_unregister_app(const char* app_id);
AppSecurityInfo security_get_app_info(const char* app_id);
SignatureStatus security_verify_signature(const char* app_id);

// Secure storage
SecureStorage* secure_storage_create(const char* key, size_t size);
void secure_storage_destroy(SecureStorage* storage);
bool secure_storage_write(SecureStorage* storage, const void* data, size_t size);
bool secure_storage_read(SecureStorage* storage, void* buffer, size_t size);
void secure_storage_clear(SecureStorage* storage);

// Security monitoring
void security_set_event_callback(SecurityEventCallback callback, void* user_data);
void security_log_event(SecurityEventType type, const char* app_id, const char* details);
SecurityEvent* security_get_recent_events(size_t* count);

// Memory protection
bool security_protect_memory_region(void* address, size_t size, uint32_t permissions);
bool security_unprotect_memory_region(void* address, size_t size);

// Secure boot and runtime verification
bool security_verify_system_integrity(void);
bool security_verify_app_integrity(const char* app_id);
bool security_is_secure_boot_enabled(void);

// Encryption utilities
bool security_encrypt_data(const void* input, size_t input_size, void* output, size_t* output_size);
bool security_decrypt_data(const void* input, size_t input_size, void* output, size_t* output_size);
void security_generate_key(uint8_t* key, size_t key_size);

#endif // SECURITY_H
