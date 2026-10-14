#include <errno.h>
#include <zephyr/kernel.h>

#include "event_service.h"

K_MUTEX_DEFINE(service_mutex);
static struct event_stats counters;
static bool initialized;
static bool running;
static int last_error;
static uint32_t sequence_number;

int event_service_init(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	/* TODO: Initialize the complete shared service state. */
	initialized = true;
	k_mutex_unlock(&service_mutex);
	return 0;
}

void event_service_set_running(bool active)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	running = active;
	last_error = 0;
	k_mutex_unlock(&service_mutex);
}

void event_service_record_published(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	counters.cmd_published++;
	k_mutex_unlock(&service_mutex);
}

void event_service_record_received(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	counters.cmd_received++;
	k_mutex_unlock(&service_mutex);
}

void event_service_record_rejected(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	counters.cmd_rejected++;
	last_error = -EINVAL;
	k_mutex_unlock(&service_mutex);
}

uint32_t event_service_next_sequence(void)
{
	k_mutex_lock(&service_mutex, K_FOREVER);
	uint32_t next = ++sequence_number;
	k_mutex_unlock(&service_mutex);
	return next;
}

int event_service_get_status(struct event_status *status)
{
	bool ready;

	if (status == NULL) {
		return -EINVAL;
	}
	k_mutex_lock(&service_mutex, K_FOREVER);
	ready = initialized;
	*status = (struct event_status) {
		.initialized = initialized,
		.running = running,
		.last_error = last_error,
	};
	k_mutex_unlock(&service_mutex);
	return ready ? 0 : -EAGAIN;
}

int event_service_get_stats(struct event_stats *stats)
{
	bool ready;

	if (stats == NULL) {
		return -EINVAL;
	}
	k_mutex_lock(&service_mutex, K_FOREVER);
	ready = initialized;
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
	/* TODO: Reset only the counters; keep the service state. */
	k_mutex_unlock(&service_mutex);
	return -ENOSYS;
}
