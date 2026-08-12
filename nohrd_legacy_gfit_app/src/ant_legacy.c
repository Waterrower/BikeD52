#include "ant_legacy.h"

#include <stdint.h>

int ant_legacy_init(void)
{
	/*
	 * TODO with nRF5 SDK + S332:
	 * - configure ANT channel(s) through sd_ant_*
	 * - preserve current GFIT ANT+ equipment behavior
	 * - verify network key handling and ANT+ profile requirements
	 */
	return 0;
}

int ant_legacy_start(void)
{
	/* TODO: sd_ant_channel_open(). */
	return 0;
}

int ant_legacy_stop(void)
{
	/* TODO: sd_ant_channel_close(). */
	return 0;
}

void ant_legacy_process(void)
{
	/* TODO: handle ANT events from the SoftDevice event dispatch path. */
}

void ant_legacy_update_bike_data(uint16_t cadence_rpm_x2,
				 uint16_t power_watts,
				 uint16_t resistance_level)
{
	(void)cadence_rpm_x2;
	(void)power_watts;
	(void)resistance_level;

	/* TODO: encode ANT+ data pages matching the existing GFIT behavior. */
}
