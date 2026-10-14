#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"

LOG_MODULE_REGISTER(event_input_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	const struct event_input *input = message;

	if (chan != &event_input_chan || input == NULL) {
		LOG_ERR("invalid input event");
		return;
	}
	event_service_record_input_received();
	LOG_DBG("input sequence=%u active=%d", input->sequence, input->active);
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
