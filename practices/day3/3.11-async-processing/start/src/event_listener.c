#include <zephyr/logging/log.h>

#include "event_channel.h"

LOG_MODULE_REGISTER(event_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	/* TODO: Validate channel/message, count receipt, process, then count completion. */
	ARG_UNUSED(chan);
	ARG_UNUSED(message);
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
