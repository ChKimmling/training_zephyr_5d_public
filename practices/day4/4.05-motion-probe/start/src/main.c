#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "motion_probe.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(motion_probe_demo, LOG_LEVEL_INF);

int main(void)
{
	const struct motion_axes stimulus = {1000, -1000, 9807};
	struct motion_axes axes = {0};
	struct motion_probe probe;
	int rc = sensor_source_init();

	if (rc == 0) {
		rc = sensor_source_set_sample(&stimulus);
	}
	if (rc == 0) {
		rc = sensor_source_read(&axes);
	}
	motion_probe_build(&probe, &axes, rc, 1U, k_uptime_get());
	LOG_INF("seq=%u quality=%s age=%lld", probe.sequence,
		motion_quality_name(probe.quality),
		motion_probe_age_ms(&probe, k_uptime_get()));
	return 0;
}

