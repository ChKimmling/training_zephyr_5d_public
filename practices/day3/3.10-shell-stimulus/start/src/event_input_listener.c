#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"

LOG_MODULE_REGISTER(event_input_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	const struct event_input *input = message;

	/* TODO: Validate and account for the received event in this async context. */
	if (chan == &event_input_chan && input != NULL) {
		event_service_record_input_received();
		LOG_INF("received input sequence=%u", input->sequence);
	}
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
