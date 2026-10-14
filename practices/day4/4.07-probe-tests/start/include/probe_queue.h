#ifndef DAY4_PROBE_QUEUE_H_
#define DAY4_PROBE_QUEUE_H_

#include <zephyr/kernel.h>

#include "motion_probe.h"

#define PROBE_QUEUE_CAPACITY 4

void probe_queue_reset(void);
int probe_queue_publish_latest(const struct motion_probe *probe);
int probe_queue_take(struct motion_probe *probe, k_timeout_t timeout);
uint32_t probe_queue_depth(void);

#endif

