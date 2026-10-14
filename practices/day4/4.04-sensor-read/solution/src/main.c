#include <errno.h>
#include <zephyr/logging/log.h>
#include "sensor_source.h"

LOG_MODULE_REGISTER(sensor_read, LOG_LEVEL_INF);

int main(void)
{
	const struct motion_axes stimuli[] = {
		{0, 0, 9807}, {1250, -750, 9500}, {-2000, 500, 9000},
	};
	struct motion_axes actual = {0};
	int rc = sensor_source_init();

	if (rc < 0) {
		LOG_ERR("sensor init failed: %d", rc);
		return rc;
	}
	for (size_t i = 0; i < ARRAY_SIZE(stimuli); ++i) {
		rc = sensor_source_set_sample(&stimuli[i]);
		rc = rc < 0 ? rc : sensor_source_read(&actual);
		if (rc < 0) {
			LOG_ERR("stimulus %u failed: %d", (unsigned int)i, rc);
			return rc;
		}
		LOG_INF("sample=%u x=%d y=%d z=%d mm/s2", (unsigned int)i,
			actual.x_mm_s2, actual.y_mm_s2, actual.z_mm_s2);
	}
	sensor_source_fail_next_read(true);
	rc = sensor_source_read(&actual);
	if (rc != -EIO) {
		LOG_ERR("expected injected -EIO, got %d", rc);
		return -EINVAL;
	}
	LOG_INF("SENSOR_READ PASS");
	return 0;
}

