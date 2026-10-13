#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "event_service.h"

static atomic_t raw_count;
static atomic_t processed_count;
K_SEM_DEFINE(event_pending, 0, UINT_MAX);

static void event_worker(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		k_sem_take(&event_pending, K_FOREVER);
		atomic_inc(&processed_count);
	}
}

K_THREAD_DEFINE(event_worker_thread, 1024, event_worker, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);

int event_service_init(void)
{
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	return 0;
}

void event_service_record_raw(void)
{
	atomic_inc(&raw_count);
	k_sem_give(&event_pending);
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}
	stats->raw_count = (uint32_t)atomic_get(&raw_count);
	stats->processed_count = (uint32_t)atomic_get(&processed_count);
	return 0;
}

int event_service_reset_stats(void)
{
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	return 0;
}
