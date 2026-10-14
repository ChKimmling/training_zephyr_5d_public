#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"
#include "connectivity.h"
#include "motion_probe.h"
#include "probe_queue.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(connectivity_app, LOG_LEVEL_INF);

int main(void)
{
	struct app_state_snapshot state;
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
	/* TODO(4.10-06): Während des Disconnects mindestens drei Proben über
	 * die unveränderte Sensor-Pipeline erfassen und anschließend zeigen,
	 * dass die Sequenz fortgeschritten ist. */
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	app_state_get(&state);
	LOG_INF("connected=%d connect=%u disconnect=%u processed=%u rc=%d",
		state.connected, state.connect_transitions, state.disconnect_transitions,
		state.processed, rc);
	return 0;
}
