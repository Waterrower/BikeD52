#include "ble_legacy.h"

#include <stdint.h>

int ble_legacy_init(const char *device_name)
{
	(void)device_name;

	/*
	 * TODO with nRF5 SDK + S332:
	 * - configure GAP name and appearance
	 * - configure GATT/ATT MTU conservatively
	 * - add FTMS or legacy fitness services
	 * - avoid forced bonding/security for normal training data
	 * - avoid whitelist-only reconnect behavior
	 */
	return 0;
}

int ble_legacy_start_advertising(void)
{
	/*
	 * TODO: ble_advertising_start() or sd_ble_gap_adv_start(), depending on
	 * the old SDK version and desired amount of control.
	 */
	return 0;
}

int ble_legacy_stop_advertising(void)
{
	/* TODO: sd_ble_gap_adv_stop(). */
	return 0;
}

void ble_legacy_process(void)
{
	/* TODO: process deferred BLE service updates if needed. */
}

void ble_legacy_update_bike_data(uint16_t cadence_rpm_x2,
				 uint16_t power_watts,
				 uint16_t resistance_level)
{
	(void)cadence_rpm_x2;
	(void)power_watts;
	(void)resistance_level;

	/* TODO: encode and notify FTMS Indoor Bike Data over sd_ble_gatts_hvx(). */
}
