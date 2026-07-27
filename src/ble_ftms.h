#ifndef BLE_FTMS_H
#define BLE_FTMS_H

#include <stdbool.h>
#include <stdint.h>

int ble_ftms_init(void);
int ble_ftms_set_enabled(bool enabled);
int ble_ftms_last_error(void);
bool ble_ftms_is_enabled(void);
bool ble_ftms_is_connected(void);
int ble_ftms_pause_advertising(void);
int ble_ftms_resume_advertising(void);
int ble_ftms_notify_indoor_bike(uint16_t speed_centi_kmh, uint16_t cadence_half_rpm,
				int16_t resistance_percent, int16_t power_watt);

#endif
