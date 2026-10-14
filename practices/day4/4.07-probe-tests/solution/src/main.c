#include <errno.h>
#include <zephyr/ztest.h>
#include "app_state.h"
#include "motion_probe.h"
#include "probe_queue.h"

static void before_test(void *fixture)
{
	ARG_UNUSED(fixture);
	probe_queue_reset();
	app_state_reset();
}

ZTEST(probe, test_valid_and_range)
{
	struct motion_probe value;
	const struct motion_axes valid = {1000, -1000, 9807};
	const struct motion_axes invalid = {20001, 0, 0};

	motion_probe_build(&value, &valid, 0, 1, 10);
	zassert_equal(value.quality, MOTION_QUALITY_OK);
	zassert_equal(value.sequence, 1U);
	motion_probe_build(&value, &invalid, 0, 2, 20);
	zassert_equal(value.quality, MOTION_QUALITY_RANGE);
}

ZTEST(probe, test_sensor_error)
{
	struct motion_probe value;
	struct motion_axes ignored = {1, 2, 3};

	motion_probe_build(&value, &ignored, -EIO, 7, 100);
	zassert_equal(value.quality, MOTION_QUALITY_SENSOR_ERROR);
	zassert_equal(value.x_mm_s2, 0);
}

ZTEST(probe, test_queue_overload_keeps_latest)
{
	struct motion_probe value = { .quality = MOTION_QUALITY_OK };
	struct app_state_snapshot state;
	int drops = 0;

	for (uint32_t sequence = 1; sequence <= 6; ++sequence) {
		value.sequence = sequence;
		if (probe_queue_publish_latest(&value) == 1) {
			drops++;
			app_state_note_queue_drop();
		}
	}
	while (probe_queue_take(&value, K_NO_WAIT) == 0) {
		app_state_accept_probe(&value);
	}
	app_state_get(&state);
	zassert_equal(drops, 2);
	zassert_equal(state.queue_drops, 2U);
	zassert_equal(state.processed, 4U);
	zassert_equal(state.last_probe.sequence, 6U);
}

ZTEST(probe, test_sequence_gap)
{
	struct motion_probe value = { .quality = MOTION_QUALITY_OK };
	struct app_state_snapshot state;

	value.sequence = 1;
	app_state_accept_probe(&value);
	value.sequence = 4;
	app_state_accept_probe(&value);
	app_state_get(&state);
	zassert_equal(state.sequence_gaps, 2U);
}

ZTEST(probe, test_age_is_never_negative)
{
	const struct motion_probe value = { .captured_ms = 100 };
	zassert_equal(motion_probe_age_ms(&value, 90), 0);
	zassert_equal(motion_probe_age_ms(&value, 125), 25);
}

ZTEST_SUITE(probe, NULL, NULL, before_test, NULL, NULL);

