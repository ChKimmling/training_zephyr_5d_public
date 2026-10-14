#include <errno.h>
#include <zephyr/kernel.h>

#include "event_service.h"

static struct k_spinlock service_lock;
static struct event_stats counters;
static bool initialized;
static bool running;
static bool input_ready;
static uint32_t sequence_number;

static void increment_counter(uint32_t *counter)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	(*counter)++;
	k_spin_unlock(&service_lock, key);
}

int event_service_init(void)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	/* TODO: Initialize every service field before publishing readiness. */
	counters = (struct event_stats){0};
	sequence_number = 0U;
	running = false;
	input_ready = false;
	initialized = true;
	k_spin_unlock(&service_lock, key);
	return 0;
}

void event_service_set_running(bool active)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	running = active;
	k_spin_unlock(&service_lock, key);
}

bool event_service_is_running(void)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);
	bool active = running;

	k_spin_unlock(&service_lock, key);
	return active;
}

void event_service_record_command_published(void) { increment_counter(&counters.cmd_published); }
void event_service_record_command_received(void) { increment_counter(&counters.cmd_received); }
void event_service_record_input_published(void) { increment_counter(&counters.input_published); }
void event_service_record_input_received(void) { increment_counter(&counters.input_received); }
void event_service_record_processed(void) { increment_counter(&counters.processed); }

void event_service_record_publish_failure(int error)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	counters.publish_failed++;
	counters.last_error = error;
	k_spin_unlock(&service_lock, key);
}

uint32_t event_service_next_sequence(void)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);
	uint32_t next = ++sequence_number;

	k_spin_unlock(&service_lock, key);
	return next;
}

void event_service_set_input_ready(bool ready)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	input_ready = ready;
	k_spin_unlock(&service_lock, key);
}

void event_service_reject(void)
{
	increment_counter(&counters.cmd_rejected);
}

void event_service_set_error(int error)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	counters.last_error = error;
	k_spin_unlock(&service_lock, key);
}

int event_service_get_status(struct event_status *status)
{
	bool ready;
	k_spinlock_key_t key;

	if (status == NULL) {
		return -EINVAL;
	}
	key = k_spin_lock(&service_lock);
	ready = initialized;
	status->initialized = initialized;
	status->running = running;
	status->input_ready = input_ready;
	status->input_active = false;
	status->stimulus_state = "idle";
	status->remaining = 0U;
	status->last_error = counters.last_error;
	k_spin_unlock(&service_lock, key);
	return ready ? 0 : -EAGAIN;
}

int event_service_get_stats(struct event_stats *stats)
{
	bool ready;
	k_spinlock_key_t key;

	if (stats == NULL) {
		return -EINVAL;
	}
	key = k_spin_lock(&service_lock);
	ready = initialized;
	*stats = counters;
	k_spin_unlock(&service_lock, key);
	return ready ? 0 : -EAGAIN;
}

int event_service_reset_stats(void)
{
	k_spinlock_key_t key = k_spin_lock(&service_lock);

	if (!initialized) {
		k_spin_unlock(&service_lock, key);
		return -EAGAIN;
	}
	/* TODO: Reset the agreed counters without changing running/device state. */
	counters = (struct event_stats){0};
	k_spin_unlock(&service_lock, key);
	return 0;
}
