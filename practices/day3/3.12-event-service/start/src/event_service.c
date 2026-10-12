#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "event_service.h"

static atomic_t raw_count;
static atomic_t processed_count;
static atomic_t rejected_count;
static atomic_t lost_count;

int event_service_init(void)
{
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	atomic_set(&rejected_count, 0);
	atomic_set(&lost_count, 0);
	/* TODO: Queue und Event-Worker initialisieren. */
	return 0;
}

void event_service_record_input_event(void)
{
	/*
	 * TODO: Callback-Kontext darf nicht blockieren. Zähle das Rohevent
	 * atomar und stelle Metadaten mit K_NO_WAIT in eine begrenzte Queue.
	 */
	atomic_inc(&raw_count);
}

void event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return;
	}
	stats->raw_count = (uint32_t)atomic_get(&raw_count);
	stats->processed_count = (uint32_t)atomic_get(&processed_count);
	stats->rejected_count = (uint32_t)atomic_get(&rejected_count);
	stats->lost_count = (uint32_t)atomic_get(&lost_count);
}

int event_service_reset_stats(void)
{
	/* TODO: Queue-/Worker-Rennen berücksichtigen; nur Statistik zurücksetzen. */
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	atomic_set(&rejected_count, 0);
	atomic_set(&lost_count, 0);
	return 0;
}
