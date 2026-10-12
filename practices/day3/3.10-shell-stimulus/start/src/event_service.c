#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "event_service.h"

static atomic_t raw_count;
static atomic_t processed_count;

int event_service_init(void)
{
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	return 0;
}

void event_service_record_raw(void)
{
	/* TODO: Callback minimal halten und Verarbeitung asynchron signalisieren. */
	atomic_inc(&raw_count);
	atomic_inc(&processed_count);
}

void event_service_get_stats(struct event_stats *stats)
{
	/* TODO: NULL prüfen; hier nur ein einfaches atomares Snapshot-Beispiel. */
	if (stats != NULL) {
		stats->raw_count = (uint32_t)atomic_get(&raw_count);
		stats->processed_count = (uint32_t)atomic_get(&processed_count);
	}
}

void event_service_reset_stats(void)
{
	/* TODO: Statistik zurücksetzen, ohne Service- oder GPIO-Zustand zu ändern. */
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
}
