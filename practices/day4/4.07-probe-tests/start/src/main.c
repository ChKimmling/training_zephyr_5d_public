#include <zephyr/ztest.h>
#include "app_state.h"
#include "motion_probe.h"
#include "probe_queue.h"

static void before_test(void *fixture)
{
	ARG_UNUSED(fixture);
	/* TODO(4.07-01): Queue und app_state vor jedem Test zurücksetzen. */
}

ZTEST(probe, test_valid_and_range)
{
	/* TODO(4.07-02): Eine gültige Probe sowie den Grenzwert ±20000 mm/s²
	 * mit fachlichen Assertions prüfen. */
	ztest_test_skip();
}

ZTEST(probe, test_sensor_error)
{
	/* TODO(4.07-03): Einen negativen Sensor-Rückgabewert injizieren und
	 * SENSOR_ERROR sowie genullte Achsen prüfen. */
	ztest_test_skip();
}

ZTEST(probe, test_queue_overload_keeps_latest)
{
	/* TODO(4.07-04): Sechs Proben in die Queue mit Kapazität vier stellen
	 * und zwei Drops sowie die neueste Sequenz prüfen. */
	ztest_test_skip();
}

ZTEST(probe, test_sequence_gap)
{
	/* TODO(4.07-05): Sequenzen 1 und 4 verarbeiten und zwei fehlende
	 * Sequenzen ohne Logvergleich nachweisen. */
	ztest_test_skip();
}

ZTEST_SUITE(probe, NULL, NULL, before_test, NULL, NULL);
