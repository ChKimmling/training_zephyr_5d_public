#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"
#include "connectivity.h"
#include "motion_probe.h"
#include "probe_queue.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(connectivity_app, LOG_LEVEL_INF);

static int capture(uint32_t sequence)
{
	const struct motion_axes stimulus = {(int32_t)sequence * 100, 0, 9807};
	struct motion_axes axes = {0};
	struct motion_probe probe;
	int rc = sensor_source_set_sample(&stimulus);

	rc = rc < 0 ? rc : sensor_source_read(&axes);
	if (rc < 0) {
		app_state_note_sensor_error();
	}
	motion_probe_build(&probe, &axes, rc, sequence, k_uptime_get());
	return probe_queue_publish_latest(&probe);
}

int main(void)
{
	struct app_state_snapshot state;
	struct motion_probe probe;
	int rc = sensor_source_init();

	app_state_reset();
	probe_queue_reset();
	if (rc == 0) {
		rc = connectivity_init();
	}
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	connectivity_emit_for_test(false);
	connectivity_wait_for_work();
	for (uint32_t sequence = 1; rc == 0 && sequence <= 3; ++sequence) {
		rc = capture(sequence);
	}
	while (probe_queue_take(&probe, K_NO_WAIT) == 0) {
		app_state_accept_probe(&probe);
	}
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	app_state_get(&state);
	LOG_INF("connected=%d connect=%u disconnect=%u seq=%u processed=%u",
		state.connected, state.connect_transitions, state.disconnect_transitions,
		state.last_probe.sequence, state.processed);
	if (!state.connected || state.connect_transitions != 2U ||
	    state.disconnect_transitions != 1U || state.last_probe.sequence != 3U) {
		LOG_ERR("CONNECTIVITY FAIL");
		return -1;
	}
	LOG_INF("CONNECTIVITY PASS");
	return 0;
}

