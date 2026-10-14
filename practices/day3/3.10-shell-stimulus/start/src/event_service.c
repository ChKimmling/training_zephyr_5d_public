#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "event_service.h"

static atomic_t cmd_published;
static atomic_t cmd_received;
static atomic_t cmd_rejected;
static atomic_t input_published;
static atomic_t input_received;
static atomic_t processed;
static atomic_t publish_failed;
static atomic_t last_error;
K_SEM_DEFINE(event_pending, 0, UINT_MAX);

static void event_worker(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
	while (true) {
		k_sem_take(&event_pending, K_FOREVER);
		/* TODO: Perform the application work before incrementing processed. */
	}
}

K_THREAD_DEFINE(event_worker_thread, 1024, event_worker, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);

int event_service_init(void)
{
	/* TODO: Reset the complete stable statistics set. */
	return 0;
}

void event_service_record_command_published(void) { atomic_inc(&cmd_published); }
void event_service_record_command_received(void) { atomic_inc(&cmd_received); }

void event_service_reject(int error)
{
	atomic_inc(&cmd_rejected);
	atomic_set(&last_error, error);
}

void event_service_record_input_published(void) { atomic_inc(&input_published); }

void event_service_record_input_received(void)
{
	atomic_inc(&input_received);
	k_sem_give(&event_pending);
}

void event_service_record_publish_failure(int error)
{
	atomic_inc(&publish_failed);
	atomic_set(&last_error, error);
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}
	stats->cmd_published = (uint32_t)atomic_get(&cmd_published);
	stats->cmd_received = (uint32_t)atomic_get(&cmd_received);
	stats->cmd_rejected = (uint32_t)atomic_get(&cmd_rejected);
	stats->input_published = (uint32_t)atomic_get(&input_published);
	stats->input_received = (uint32_t)atomic_get(&input_received);
	stats->processed = (uint32_t)atomic_get(&processed);
	stats->publish_failed = (uint32_t)atomic_get(&publish_failed);
	stats->last_error = (int32_t)atomic_get(&last_error);
	return 0;
}

int event_service_reset_stats(void)
{
	/* TODO: Reset all counters while leaving the active stimulus untouched. */
	return -ENOSYS;
}
