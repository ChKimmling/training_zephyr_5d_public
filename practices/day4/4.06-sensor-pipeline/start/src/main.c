#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"
#include "motion_probe.h"
#include "probe_queue.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(motion_monitor, LOG_LEVEL_INF);

int main(void)
{
	const struct motion_axes stimulus = {0, 0, 9807};
	struct motion_axes axes = {0};
	struct motion_probe probe;
	struct app_state_snapshot state;
	int rc = sensor_source_init();

	probe_queue_reset();
	app_state_reset();
	if (rc == 0) {
		rc = sensor_source_set_sample(&stimulus);
	}
	if (rc == 0) {
		rc = sensor_source_read(&axes);
	}
	motion_probe_build(&probe, &axes, rc, 1U, k_uptime_get());
	rc = probe_queue_publish_latest(&probe);
	if (rc >= 0 && probe_queue_take(&probe, K_NO_WAIT) == 0) {
		app_state_accept_probe(&probe);
	}
	app_state_get(&state);
	LOG_INF("processed=%u drops=%u gaps=%u", state.processed,
		state.queue_drops, state.sequence_gaps);
	return 0;
}
