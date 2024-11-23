#ifndef CEREBRO_OS_HAL_CLOCK_HAL_H
#define CEREBRO_OS_HAL_CLOCK_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// RTC Configuration
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    bool twenty_four_hour;
    int8_t timezone_offset;  // in hours
} RTCConfig;

// RTC Alarm Configuration
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day_mask;  // Bit field for days of week
    bool enabled;
    void (*callback)(void* data);
    void* callback_data;
} RTCAlarm;

// Hardware Timer Configuration
typedef struct {
    uint32_t interval_ms;
    bool repeat;
    void (*callback)(void* data);
    void* callback_data;
} TimerConfig;

// Power Management
typedef enum {
    CLOCK_POWER_NORMAL,
    CLOCK_POWER_SAVING,
    CLOCK_POWER_CRITICAL
} ClockPowerMode;

// Error Codes
typedef enum {
    CLOCK_HAL_SUCCESS = 0,
    CLOCK_HAL_ERROR_INIT = -1,
    CLOCK_HAL_ERROR_PARAM = -2,
    CLOCK_HAL_ERROR_HARDWARE = -3,
    CLOCK_HAL_ERROR_BUSY = -4,
    CLOCK_HAL_ERROR_TIMEOUT = -5
} ClockHALError;

// RTC Functions
ClockHALError hal_rtc_init(const RTCConfig* config);
ClockHALError hal_rtc_get_time(struct tm* time);
ClockHALError hal_rtc_set_time(const struct tm* time);
ClockHALError hal_rtc_set_alarm(uint8_t alarm_id, const RTCAlarm* alarm);
ClockHALError hal_rtc_get_alarm(uint8_t alarm_id, RTCAlarm* alarm);
ClockHALError hal_rtc_enable_alarm(uint8_t alarm_id, bool enable);
ClockHALError hal_rtc_clear_alarm(uint8_t alarm_id);

// Timer Functions
ClockHALError hal_timer_init(void);
ClockHALError hal_timer_start(uint8_t timer_id, const TimerConfig* config);
ClockHALError hal_timer_stop(uint8_t timer_id);
ClockHALError hal_timer_get_remaining(uint8_t timer_id, uint32_t* remaining_ms);
ClockHALError hal_timer_is_active(uint8_t timer_id, bool* active);

// Power Management Functions
ClockHALError hal_clock_set_power_mode(ClockPowerMode mode);
ClockHALError hal_clock_get_power_mode(ClockPowerMode* mode);
ClockHALError hal_clock_enable_wakeup(bool enable);

// Utility Functions
ClockHALError hal_clock_get_uptime(uint64_t* uptime_ms);
ClockHALError hal_clock_get_battery_status(uint8_t* battery_percent);
bool hal_clock_is_charging(void);

#endif // CEREBRO_OS_HAL_CLOCK_HAL_H
