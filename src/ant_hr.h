#ifndef ANT_HR_H
#define ANT_HR_H

#include <stdbool.h>
#include <stdint.h>

enum ant_hr_state {
	ANT_HR_IDLE,
	ANT_HR_SEARCHING,
	ANT_HR_CONNECTED,
	ANT_HR_FAILED,
	ANT_HR_UNAVAILABLE,
};

int ant_hr_init(void);
int ant_hr_start_pairing(void);
int ant_hr_disconnect(void);
void ant_hr_process(void);
enum ant_hr_state ant_hr_get_state(void);
bool ant_hr_is_connected(void);
uint8_t ant_hr_get_bpm(void);
const char *ant_hr_get_name(void);

#endif
