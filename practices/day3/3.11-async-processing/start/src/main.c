#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channel.h"

LOG_MODULE_REGISTER(day3_async_processing, LOG_LEVEL_INF);

int main(void)
{
	for (uint32_t i = 1U; i <= 5U; i++) {
		struct event_input input = {
			.source = EVENT_SOURCE_EMULATED,
			.sequence = i,
			.active = true,
			.timestamp = k_uptime_get(),
		};
		int ret = zbus_chan_pub(&event_input_chan, &input, K_NO_WAIT);

		if (ret != 0) {
			LOG_ERR("input publish failed: %d", ret);
			return ret;
		}
	}
	LOG_INF("published five input events");
	return 0;
}
