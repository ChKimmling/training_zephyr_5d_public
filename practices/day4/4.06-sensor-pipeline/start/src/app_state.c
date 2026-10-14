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
	/* TODO(4.06-03): sensor_errors unter dem State-Lock genau einmal
	 * erhöhen. */
}

void app_state_note_queue_drop(void)
{
	/* Der Drop-Zähler gehört zum selben Zähler-Arbeitsauftrag. */
}

void app_state_accept_probe(const struct motion_probe *probe)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	/* TODO(4.06-06): Vor dem Überschreiben eine Sequenzlücke erkennen und
	 * nur die tatsächlich fehlenden Sequenzen zählen. */
	/* TODO(4.06-05): Probe als konsistenten letzten Snapshot speichern,
	 * has_probe setzen und processed erhöhen. */
	ARG_UNUSED(probe);
	k_mutex_unlock(&state_lock);
}

void app_state_get(struct app_state_snapshot *snapshot)
{
	k_mutex_lock(&state_lock, K_FOREVER);
	*snapshot = state;
	k_mutex_unlock(&state_lock);
}

