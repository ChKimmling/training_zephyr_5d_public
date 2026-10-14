#include "app_state.h"

#include <string.h>

#include <zephyr/kernel.h>

static K_MUTEX_DEFINE(state_lock);
static struct app_state_snapshot state;

void app_state_reset(void)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	memset(&state, 0, sizeof(state));
	k_mutex_unlock(&state_lock);
}

void app_state_note_sensor_error(void)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	state.sensor_errors++;
	k_mutex_unlock(&state_lock);
}

void app_state_note_queue_drop(void)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	state.queue_drops++;
	k_mutex_unlock(&state_lock);
}

void app_state_accept_probe(const struct motion_probe *probe)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	if (state.has_probe && probe->sequence > state.last_probe.sequence + 1U) {
		state.sequence_gaps += probe->sequence - state.last_probe.sequence - 1U;
	}
	state.last_probe = *probe;
	state.has_probe = true;
	state.processed++;
	k_mutex_unlock(&state_lock);
}

void app_state_get(struct app_state_snapshot *snapshot)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	*snapshot = state;
	k_mutex_unlock(&state_lock);
}

