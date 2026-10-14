#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "app_state.h"
#include "connectivity.h"
#include "probe_queue.h"
#include "sensor_source.h"

LOG_MODULE_REGISTER(tcp_status, LOG_LEVEL_INF);

int main(void)
{
	struct app_state_snapshot state;
	int rc = sensor_source_init();

	app_state_reset();
	probe_queue_reset();
	if (rc == 0) rc = connectivity_init();
	connectivity_emit_for_test(true);
	connectivity_wait_for_work();
	app_state_get(&state);
	/* TODO(4.12-01): Listener nur bei erreichtem L4-Zustand öffnen und
	 * andernfalls die Sensor-Pipeline ohne Transport weiterlaufen lassen. */
	/* TODO(4.12-05): Nach Disconnect, weiterer Sensorerfassung und Reconnect
	 * dieselbe Listener-Instanz für eine zweite Anfrage verwenden. */
	LOG_INF("integration start ready: connected=%d rc=%d", state.connected, rc);
	return 0;
}
