#ifndef BLE_FTMS_H
#define BLE_FTMS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef bool (*ble_ftms_calibration_status_cb_t)(void);
typedef int (*ble_ftms_calibration_set_cb_t)(uint8_t value_id,
					     const uint8_t *data,
					     size_t len);

int ble_ftms_init(void);
int ble_ftms_set_enabled(bool enabled);
int ble_ftms_last_error(void);
bool ble_ftms_is_enabled(void);
bool ble_ftms_is_connected(void);
int ble_ftms_disconnect(void);
void ble_ftms_set_calibration_handlers(ble_ftms_calibration_status_cb_t status_cb,
				       ble_ftms_calibration_set_cb_t set_cb);
int ble_ftms_pause_advertising(void);
int ble_ftms_resume_advertising(void);
int ble_ftms_hrs_set_active(bool active);
int ble_ftms_notify_heart_rate(uint8_t bpm);
int ble_ftms_notify_session_started(void);
int ble_ftms_notify_session_ended(void);
int ble_ftms_notify_indoor_bike(uint16_t speed_centi_kmh, uint16_t cadence_half_rpm,
				int16_t resistance_percent, int16_t power_watt);

#endif
