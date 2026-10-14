#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"

LOG_MODULE_REGISTER(stimulus_subscriber, LOG_LEVEL_INF);

ZBUS_MSG_SUBSCRIBER_DEFINE(event_stimulus_subscriber);

static void subscriber_thread(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *chan;
	struct event_command command;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	while (zbus_sub_wait_msg(&event_stimulus_subscriber, &chan, &command, K_FOREVER) == 0) {
		/* TODO: Route each validated command to the stimulus/service API. */
		if (chan == &event_cmd_chan) {
			event_service_record_command_received();
			LOG_INF("received command type=%d sequence=%u", command.type, command.sequence);
		}
	}
}

K_THREAD_DEFINE(event_stimulus_subscriber_thread, 2048, subscriber_thread,
		NULL, NULL, NULL, K_PRIO_PREEMPT(3), 0, 0);
