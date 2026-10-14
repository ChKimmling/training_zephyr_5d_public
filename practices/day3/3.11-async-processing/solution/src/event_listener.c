#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "event_channel.h"

LOG_MODULE_REGISTER(event_listener, LOG_LEVEL_INF);

static atomic_t input_received;
static atomic_t processed;

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	const struct event_input *input = message;

	if (chan != &event_input_chan || input == NULL) {
		LOG_ERR("invalid input notification");
		return;
	}
	atomic_inc(&input_received);
	LOG_INF("processing sequence=%u source=%d active=%d",
		input->sequence, input->source, input->active);
	atomic_inc(&processed);
	LOG_INF("input_received=%d processed=%d",
		atomic_get(&input_received), atomic_get(&processed));
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
