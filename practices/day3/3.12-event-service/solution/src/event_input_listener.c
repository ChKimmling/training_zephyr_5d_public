#include <zephyr/logging/log.h>

#include "event_channels.h"
#include "event_service.h"

LOG_MODULE_REGISTER(event_input_listener, LOG_LEVEL_INF);

static void input_callback(const struct zbus_channel *chan, const void *message)
{
	const struct event_input *input = message;

	if (chan != &event_input_chan || input == NULL) {
		LOG_ERR("invalid input notification");
		event_service_set_error(-EINVAL);
		return;
	}
	event_service_record_input_received();
	if (!event_service_is_running()) {
		event_service_reject();
		event_service_set_error(-ESHUTDOWN);
		LOG_WRN("input rejected while service is stopped");
		return;
	}
	LOG_INF("processed input source=%d sequence=%u active=%d",
		input->source, input->sequence, input->active);
	event_service_record_processed();
}

ZBUS_ASYNC_LISTENER_DEFINE(event_input_listener, input_callback);
