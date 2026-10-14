#include <zephyr/logging/log.h>

#include "event_channel.h"

LOG_MODULE_REGISTER(event_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	/* TODO: Validate channel/message, count receipt, process, then count completion. */
	const struct event_input *input = message;

	LOG_INF("received input sequence=%u source=%d active=%d",
		input->sequence, input->source, input->active);
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
