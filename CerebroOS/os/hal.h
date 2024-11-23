#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>

// Hardware capabilities flags
typedef enum {
    HW_CAP_DISPLAY      = 1 << 0,
    HW_CAP_KEYPAD       = 1 << 1,
    HW_CAP_TOUCHSCREEN  = 1 << 2,
    HW_CAP_CAMERA       = 1 << 3,
    HW_CAP_BLUETOOTH    = 1 << 4,
    HW_CAP_WIFI         = 1 << 5,
    HW_CAP_GSM          = 1 << 6,
    HW_CAP_GPS          = 1 << 7,
    HW_CAP_ACCELEROMETER = 1 << 8,
    HW_CAP_VIBRATOR     = 1 << 9,
    HW_CAP_SPEAKER      = 1 << 10,
    HW_CAP_MICROPHONE   = 1 << 11
} HardwareCapability;

// Display information
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;           // Bits per pixel
    bool is_color;
    bool supports_rotation;
} DisplayInfo;

// Input event types
typedef enum {
    INPUT_KEYPRESS,
    INPUT_KEYRELEASE,
    INPUT_TOUCH_DOWN,
    INPUT_TOUCH_UP,
    INPUT_TOUCH_MOVE
} InputEventType;

// Input event data
typedef struct {
    InputEventType type;
    union {
        struct {
            uint8_t keycode;
            bool is_long_press;
        } key;
        struct {
            uint16_t x;
            uint16_t y;
            uint8_t pressure;
        } touch;
    };
    uint32_t timestamp;
} InputEvent;

// Power states
typedef enum {
    POWER_STATE_ACTIVE,
    POWER_STATE_IDLE,
    POWER_STATE_SLEEP,
    POWER_STATE_DEEP_SLEEP
} PowerState;

// Battery information
typedef struct {
    uint8_t level;         // Battery level percentage
    bool is_charging;
    uint32_t voltage_mv;   // Battery voltage in millivolts
    int16_t temperature;   // Battery temperature in 0.1°C
} BatteryInfo;

// Hardware interface functions
bool hal_init(void);
uint32_t hal_get_capabilities(void);
bool hal_has_capability(HardwareCapability cap);

// Display functions
bool hal_display_init(void);
DisplayInfo hal_get_display_info(void);
bool hal_display_set_brightness(uint8_t level);
bool hal_display_set_orientation(uint8_t rotation);
bool hal_display_draw_buffer(const uint8_t* buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

// Input functions
bool hal_input_init(void);
bool hal_input_get_event(InputEvent* event);
void hal_input_flush(void);

// Power management
bool hal_set_power_state(PowerState state);
PowerState hal_get_power_state(void);
BatteryInfo hal_get_battery_info(void);
bool hal_enable_peripheral(HardwareCapability peripheral, bool enable);

// System information
uint32_t hal_get_total_ram(void);
uint32_t hal_get_free_ram(void);
uint32_t hal_get_total_storage(void);
uint32_t hal_get_free_storage(void);
uint32_t hal_get_cpu_frequency(void);
uint8_t hal_get_cpu_usage(void);
uint32_t hal_get_uptime(void);

// Error handling
typedef void (*HalErrorHandler)(const char* module, int error_code, const char* message);
void hal_set_error_handler(HalErrorHandler handler);

#endif // HAL_H
