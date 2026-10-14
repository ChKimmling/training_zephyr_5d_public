#include "probe_queue.h"
#include <errno.h>

K_MSGQ_DEFINE(motion_probe_queue, sizeof(struct motion_probe), PROBE_QUEUE_CAPACITY, 4);

void probe_queue_reset(void)
{
	k_msgq_purge(&motion_probe_queue);
}

int probe_queue_publish_latest(const struct motion_probe *probe)
{
	/* TODO(4.06-01): Die Probe mit K_NO_WAIT einstellen und Erfolg direkt
	 * zurückgeben. */
	/* TODO(4.06-02): Bei voller Queue den ältesten Eintrag entfernen, die
	 * neue Probe einstellen und einen Drop durch Rückgabewert 1 melden. */
	ARG_UNUSED(probe);
	return -EAGAIN;
}

int probe_queue_take(struct motion_probe *probe, k_timeout_t timeout)
{
	/* TODO(4.06-04): Eine vollständige Probe mit dem übergebenen Timeout
	 * aus der Queue entnehmen. */
	ARG_UNUSED(probe);
	ARG_UNUSED(timeout);
	return -ENOMSG;
}

uint32_t probe_queue_depth(void)
{
	return k_msgq_num_used_get(&motion_probe_queue);
}

