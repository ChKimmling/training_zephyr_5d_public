#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "event_service.h"

#define EVENT_QUEUE_CAPACITY 8
#define EVENT_PROCESSING_MS 250

struct queued_event {
	atomic_val_t stats_epoch;
};

K_MSGQ_DEFINE(event_queue, sizeof(struct queued_event), EVENT_QUEUE_CAPACITY, 4);

static atomic_t raw_count;
static atomic_t processed_count;
static atomic_t rejected_count;
static atomic_t lost_count;
static atomic_t input_ready;
static atomic_t last_error;
static atomic_t initialized;
static atomic_t stats_epoch;
static struct k_spinlock stats_lock;

static void event_worker(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (true) {
		struct queued_event event;
		int ret = k_msgq_get(&event_queue, &event, K_FOREVER);

		if (ret == 0) {
			/* Simuliert langsame Arbeit, damit Burst-Überlast beobachtbar ist. */
			k_msleep(EVENT_PROCESSING_MS);
			k_spinlock_key_t key = k_spin_lock(&stats_lock);

			if (event.stats_epoch == atomic_get(&stats_epoch)) {
				atomic_inc(&processed_count);
			}
			k_spin_unlock(&stats_lock, key);
		}
	}
}

K_THREAD_DEFINE(event_worker_thread, 1024, event_worker, NULL, NULL, NULL,
		K_PRIO_PREEMPT(3), 0, 0);

int event_service_init(void)
{
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	atomic_set(&rejected_count, 0);
	atomic_set(&lost_count, 0);
	atomic_set(&input_ready, 0);
	atomic_set(&last_error, 0);
	atomic_set(&initialized, 1);
	atomic_set(&stats_epoch, 0);
	return 0;
}

void event_service_set_input_ready(bool ready)
{
	atomic_set(&input_ready, ready ? 1 : 0);
}

void event_service_record_input_event(void)
{
	struct queued_event event;
	k_spinlock_key_t key = k_spin_lock(&stats_lock);

	event.stats_epoch = atomic_get(&stats_epoch);
	atomic_inc(&raw_count);
	k_spin_unlock(&stats_lock, key);

	if (k_msgq_put(&event_queue, &event, K_NO_WAIT) != 0) {
		key = k_spin_lock(&stats_lock);
		if (event.stats_epoch == atomic_get(&stats_epoch)) {
			atomic_inc(&lost_count);
		}
		k_spin_unlock(&stats_lock, key);
	}
}

void event_service_reject(void)
{
	atomic_inc(&rejected_count);
}

void event_service_set_error(int error)
{
	atomic_set(&last_error, error);
}

int event_service_get_status(struct event_status *status)
{
	if (status == NULL) {
		return -EINVAL;
	}
	status->initialized = atomic_get(&initialized) != 0;
	status->input_ready = atomic_get(&input_ready) != 0;
	status->input_active = false;
	status->stimulus_state = "idle";
	status->remaining = 0U;
	status->last_error = atomic_get(&last_error);
	return status->initialized ? 0 : -EAGAIN;
}

int event_service_get_stats(struct event_stats *stats)
{
	if (stats == NULL) {
		return -EINVAL;
	}
	k_spinlock_key_t key = k_spin_lock(&stats_lock);
	stats->raw_count = (uint32_t)atomic_get(&raw_count);
	stats->processed_count = (uint32_t)atomic_get(&processed_count);
	stats->rejected_count = (uint32_t)atomic_get(&rejected_count);
	stats->lost_count = (uint32_t)atomic_get(&lost_count);
	k_spin_unlock(&stats_lock, key);
	return atomic_get(&initialized) != 0 ? 0 : -EAGAIN;
}

int event_service_reset_stats(void)
{
	k_spinlock_key_t key;

	if (atomic_get(&initialized) == 0) {
		return -EAGAIN;
	}
	key = k_spin_lock(&stats_lock);
	atomic_inc(&stats_epoch);
	atomic_set(&raw_count, 0);
	atomic_set(&processed_count, 0);
	atomic_set(&rejected_count, 0);
	atomic_set(&lost_count, 0);
	k_spin_unlock(&stats_lock, key);
	return 0;
}
