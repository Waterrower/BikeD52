#include <stdbool.h>
#include <stdint.h>

#include "nohrd_legacy_config.h"
#include "gfit_uart.h"
#include "ble_legacy.h"
#include "ant_legacy.h"

/*
 * Legacy nRF5 SDK application skeleton.
 *
 * This file is intentionally SDK-light until the exact original SDK and
 * SoftDevice versions are known. The real implementation should include
 * nrf_sdh.h, nrf_sdh_ble.h, ant_stack_config_defs.h, app_timer.h, app_uart.h,
 * and the board-specific headers from the verified legacy SDK.
 */

static void platform_init(void)
{
	/* TODO: clocks, app_timer, GPIO, UART, logging policy. */
}

static void softdevice_init(void)
{
	/*
	 * TODO:
	 * - enable the existing ANT/BLE SoftDevice
	 * - configure BLE observer count and connection cfg
	 * - configure ANT channel resources
	 */
}

int main(void)
{
	platform_init();
	softdevice_init();

	gfit_uart_init();
	ant_legacy_init();
	ble_legacy_init(NOHRD_BLE_ADV_NAME_DEFAULT);

	ble_legacy_start_advertising();

	for (;;) {
		gfit_uart_process();
		ant_legacy_process();
		ble_legacy_process();

		/*
		 * TODO: enter low power with sd_app_evt_wait() once the
		 * SoftDevice init path is in place.
		 */
	}
}
