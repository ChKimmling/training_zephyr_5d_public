#include <zephyr/logging/log.h>

#include "event_channels.h"

LOG_MODULE_REGISTER(day3_zbus_contract, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("zbus contract ready: event_cmd_chan and event_input_chan");
	return 0;
}
