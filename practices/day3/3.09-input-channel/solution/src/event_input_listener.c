#include <zephyr/logging/log.h>

#include "event_channels.h"

LOG_MODULE_REGISTER(event_input, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan)
{
	const struct event_input *input = zbus_chan_const_msg(chan);

	LOG_INF("input source=%d sequence=%u active=%d",
		input->source, input->sequence, input->active);
}

ZBUS_LISTENER_DEFINE(event_input_listener, input_callback);
