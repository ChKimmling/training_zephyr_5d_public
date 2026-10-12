#include <zephyr/kernel.h>

#include "event_service.h"

static bool initialized;

int event_service_init(void)
{
	/* TODO: Setze einen definierten initialen Status und initialisiere den Service. */
	initialized = true;
	return 0;
}

int event_service_get_status(struct event_status *status)
{
	if (status == NULL) {
		return -EINVAL;
	}

	/* TODO: Liefere einen konsistenten Snapshot, nicht interne Variablen. */
	status->initialized = initialized;
	status->input_ready = false;
	status->input_active = false;
	status->stimulus_state = "idle";
	status->last_error = 0;
	return 0;
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}

	/* TODO: Snapshot der Zähler liefern. */
	stats->raw_count = 0;
	stats->processed_count = 0;
	stats->rejected_count = 0;
	return initialized ? 0 : -EAGAIN;
}

int event_service_reset_stats(void)
{
	/* TODO: Nur die dokumentierten Statistikfelder zurücksetzen. */
	return initialized ? 0 : -EAGAIN;
}
