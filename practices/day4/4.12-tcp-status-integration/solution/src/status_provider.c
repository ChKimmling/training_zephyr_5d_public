#include "status_provider.h"

#include <errno.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#include "app_state.h"
#include "motion_probe.h"

int status_format(char *buffer, size_t size)
{
	struct app_state_snapshot state;
	int written;

	app_state_get(&state);
	if (!state.has_probe) {
		return -EAGAIN;
	}
	written = snprintk(buffer, size,
		"seq=%u x=%d y=%d z=%d quality=%s age_ms=%lld drops=%u sensor_errors=%u connected=%d\n",
		state.last_probe.sequence, state.last_probe.x_mm_s2,
		state.last_probe.y_mm_s2, state.last_probe.z_mm_s2,
		motion_quality_name(state.last_probe.quality),
		motion_probe_age_ms(&state.last_probe, k_uptime_get()),
		state.queue_drops, state.sensor_errors, state.connected);
	return written < 0 || (size_t)written >= size ? -ENOSPC : written;
}

