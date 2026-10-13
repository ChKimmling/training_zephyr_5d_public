#include <errno.h>
#include <zephyr/kernel.h>

#include "event_service.h"

/* TODO: Protect initialization state and statistics with a Zephyr mutex. */
static bool initialized;

int event_service_init(void)
{
	/* TODO: Reset all statistics under the mutex before marking the service ready. */
	initialized = true;
	return 0;
}

int event_service_get_status(struct event_status *status)
{
	if (status == NULL) {
		return -EINVAL;
	}

	/* TODO: Copy one consistent snapshot and return -EAGAIN until initialized. */
	status->initialized = initialized;
	status->input_ready = false;
	status->input_active = false;
	status->stimulus_state = "idle";
	status->last_error = 0;
	return initialized ? 0 : -EAGAIN;
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}

	/* TODO: Copy all counters as one consistent snapshot. */
	stats->raw_count = 0;
	stats->processed_count = 0;
	stats->rejected_count = 0;
	return initialized ? 0 : -EAGAIN;
}

int event_service_reset_stats(void)
{
	/* TODO: Reset only the three statistics counters; preserve service status. */
	return initialized ? 0 : -EAGAIN;
}
