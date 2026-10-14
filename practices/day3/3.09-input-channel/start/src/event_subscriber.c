#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channels.h"

LOG_MODULE_REGISTER(event_subscriber, LOG_LEVEL_INF);

ZBUS_MSG_SUBSCRIBER_DEFINE(event_stimulus_subscriber);

static void subscriber_thread(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *chan;
	struct event_command command;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	while (zbus_sub_wait_msg(&event_stimulus_subscriber, &chan, &command, K_FOREVER) == 0) {
		/* TODO: Validate the command and create a portable input message. */
		if (chan == &event_cmd_chan) {
			struct event_input input = {
				.source = EVENT_SOURCE_EMULATED,
				.sequence = command.sequence,
				.active = command.value != 0,
				.timestamp = k_uptime_get(),
			};
			int ret = zbus_chan_pub(&event_input_chan, &input, K_NO_WAIT);

			if (ret != 0) {
				LOG_ERR("input publish failed: %d", ret);
			}
		}
	}
}

K_THREAD_DEFINE(event_subscriber_thread, 1536, subscriber_thread, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);
