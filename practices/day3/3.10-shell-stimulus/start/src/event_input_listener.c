#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"

LOG_MODULE_REGISTER(event_input_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	/* TODO: Validate and account for the received event in this async context. */
	ARG_UNUSED(chan);
	ARG_UNUSED(message);
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
