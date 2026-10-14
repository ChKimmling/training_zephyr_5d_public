#include "probe_queue.h"

#include <errno.h>

K_MSGQ_DEFINE(motion_probe_queue, sizeof(struct motion_probe), PROBE_QUEUE_CAPACITY, 4);

void probe_queue_reset(void)
{
	k_msgq_purge(&motion_probe_queue);
}

int probe_queue_publish_latest(const struct motion_probe *probe)
{
	struct motion_probe discarded;
	int rc;

	if (probe == NULL) {
		return -EINVAL;
	}

	rc = k_msgq_put(&motion_probe_queue, probe, K_NO_WAIT);
	if (rc == 0) {
		return 0;
	}
	if (rc != -ENOMSG) {
		return rc;
	}

	rc = k_msgq_get(&motion_probe_queue, &discarded, K_NO_WAIT);
	if (rc < 0) {
		return rc;
	}
	rc = k_msgq_put(&motion_probe_queue, probe, K_NO_WAIT);
	return rc < 0 ? rc : 1;
}

int probe_queue_take(struct motion_probe *probe, k_timeout_t timeout)
{
	return k_msgq_get(&motion_probe_queue, probe, timeout);
}

uint32_t probe_queue_depth(void)
{
	return k_msgq_num_used_get(&motion_probe_queue);
}

