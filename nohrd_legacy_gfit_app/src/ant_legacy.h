#ifndef ANT_LEGACY_H
#define ANT_LEGACY_H

#include <stdint.h>

int ant_legacy_init(void);
int ant_legacy_start(void);
int ant_legacy_stop(void);
void ant_legacy_process(void);
void ant_legacy_update_bike_data(uint16_t cadence_rpm_x2,
				 uint16_t power_watts,
				 uint16_t resistance_level);

#endif /* ANT_LEGACY_H */
