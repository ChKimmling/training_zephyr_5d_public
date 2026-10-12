#include <zephyr/kernel.h>

#include "event_service.h"

K_MUTEX_DEFINE(service_mutex);
static struct event_stats counters;
static bool initialized;

int event_service_init(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	counters = (struct event_stats){0};
	initialized = true;
	k_mutex_unlock(&service_mutex);
	return 0;
}

int event_service_get_status(struct event_status *status)
{
	if (status == NULL) {
		return -EINVAL;
	}
	k_mutex_lock(&service_mutex, K_FOREVER);
	bool ready = initialized;
	*status = (struct event_status){
		.initialized = ready,
		.input_ready = false,
		.input_active = false,
		.stimulus_state = "idle",
		.last_error = 0,
	};
	k_mutex_unlock(&service_mutex);
	return ready ? 0 : -EAGAIN;
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}
	k_mutex_lock(&service_mutex, K_FOREVER);
	bool ready = initialized;
	*stats = counters;
	k_mutex_unlock(&service_mutex);
	return ready ? 0 : -EAGAIN;
}

int event_service_reset_stats(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	if (!initialized) {
		k_mutex_unlock(&service_mutex);
		return -EAGAIN;
	}
	counters = (struct event_stats){0};
	k_mutex_unlock(&service_mutex);
	return 0;
}
