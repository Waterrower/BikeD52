#ifndef BLE_LEGACY_H
#define BLE_LEGACY_H

#include <stdint.h>

int ble_legacy_init(const char *device_name);
int ble_legacy_start_advertising(void);
int ble_legacy_stop_advertising(void);
void ble_legacy_process(void);
void ble_legacy_update_bike_data(uint16_t cadence_rpm_x2,
				 uint16_t power_watts,
				 uint16_t resistance_level);

#endif /* BLE_LEGACY_H */
