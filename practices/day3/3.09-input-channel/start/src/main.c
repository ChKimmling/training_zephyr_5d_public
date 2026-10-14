#include <zephyr/logging/log.h>

#include "event_channels.h"

LOG_MODULE_REGISTER(day3_input_channel, LOG_LEVEL_INF);

int main(void)
{
	struct event_command command = {
		.type = EVENT_CMD_INPUT,
		.sequence = 1U,
		.value = 1,
	};
	int ret = zbus_chan_pub(&event_cmd_chan, &command, K_NO_WAIT);

	if (ret != 0) {
		LOG_ERR("command publish failed: %d", ret);
		return ret;
	}
	LOG_INF("test command published");
	return 0;
}
