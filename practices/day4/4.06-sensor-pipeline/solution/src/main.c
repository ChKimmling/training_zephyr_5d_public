#include <errno.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "app_state.h"
#include "motion_probe.h"
#include "probe_queue.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(motion_monitor, LOG_LEVEL_INF);

static int capture(uint32_t sequence, const struct motion_axes *stimulus)
{
	struct motion_axes axes = {0};
	struct motion_probe probe;
	int rc;
	int queue_rc;

	rc = sensor_source_set_sample(stimulus);
	if (rc < 0) {
		return rc;
	}
	rc = sensor_source_read(&axes);
	if (rc < 0) {
		app_state_note_sensor_error();
	}
	motion_probe_build(&probe, &axes, rc, sequence, k_uptime_get());
	queue_rc = probe_queue_publish_latest(&probe);
	if (queue_rc == 1) {
		app_state_note_queue_drop();
		return 0;
	}
	return queue_rc;
}

int main(void)
{
	const struct motion_axes stimuli[] = {
		{0, 0, 9807}, {1000, -1000, 9807}, {2000, 0, 9700},
		{3000, 0, 9600}, {4000, 0, 9500}, {5000, 0, 9400},
	};
	struct app_state_snapshot snapshot;
	struct motion_probe probe;
	int rc;

	rc = sensor_source_init();
	if (rc < 0) {
		LOG_ERR("sensor init failed: %d", rc);
		return rc;
	}
	probe_queue_reset();
	app_state_reset();

	for (size_t i = 0; i < ARRAY_SIZE(stimuli); ++i) {
		rc = capture((uint32_t)i + 1U, &stimuli[i]);
		if (rc < 0) {
			LOG_ERR("capture failed: %d", rc);
			return rc;
		}
	}

	while (probe_queue_take(&probe, K_NO_WAIT) == 0) {
		app_state_accept_probe(&probe);
	}
	app_state_get(&snapshot);

	LOG_INF("STATUS seq=%u quality=%s processed=%u drops=%u gaps=%u sensor_errors=%u",
		snapshot.last_probe.sequence, motion_quality_name(snapshot.last_probe.quality),
		snapshot.processed, snapshot.queue_drops, snapshot.sequence_gaps,
		snapshot.sensor_errors);
	if (snapshot.last_probe.sequence != 6U || snapshot.processed != 4U ||
	    snapshot.queue_drops != 2U || snapshot.sequence_gaps != 0U) {
		LOG_ERR("PIPELINE FAIL");
		return -EINVAL;
	}
	LOG_INF("PIPELINE PASS");
	return 0;
}
