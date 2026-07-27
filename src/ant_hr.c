#include "ant_hr.h"

#include <errno.h>

static enum ant_hr_state ant_state = ANT_HR_UNAVAILABLE;

int ant_hr_init(void)
{
	return -ENOTSUP;
}

int ant_hr_start_pairing(void)
{
	ant_state = ANT_HR_UNAVAILABLE;
	return -ENOTSUP;
}

int ant_hr_disconnect(void)
{
	ant_state = ANT_HR_UNAVAILABLE;
	return 0;
}

void ant_hr_process(void)
{
}

enum ant_hr_state ant_hr_get_state(void)
{
	return ant_state;
}

bool ant_hr_is_connected(void)
{
	return ant_state == ANT_HR_CONNECTED;
}

uint8_t ant_hr_get_bpm(void)
{
	return 0U;
}

const char *ant_hr_get_name(void)
{
	return "ANT+";
}
