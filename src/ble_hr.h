#ifndef BLE_HR_H
#define BLE_HR_H

#include <stdbool.h>
#include <stdint.h>

enum ble_hr_state {
	BLE_HR_IDLE,
	BLE_HR_SCANNING,
	BLE_HR_CONNECTING,
	BLE_HR_CONNECTED,
	BLE_HR_FAILED,
};

int ble_hr_start_pairing(void);
int ble_hr_disconnect(void);
void ble_hr_process(void);
enum ble_hr_state ble_hr_get_state(void);
bool ble_hr_is_connected(void);
uint8_t ble_hr_get_bpm(void);
int8_t ble_hr_get_rssi(void);
const char *ble_hr_get_name(void);

#endif
