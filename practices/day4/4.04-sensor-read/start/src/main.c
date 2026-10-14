#include <zephyr/logging/log.h>
#include "sensor_source.h"

LOG_MODULE_REGISTER(sensor_read, LOG_LEVEL_INF);

int main(void)
{
	const struct motion_axes stimulus = {1250, -750, 9500};
	struct motion_axes actual = {0};
	int rc = sensor_source_init();

	if (rc == 0) {
		rc = sensor_source_set_sample(&stimulus);
	}
	if (rc == 0) {
		rc = sensor_source_read(&actual);
	}
	if (rc < 0) {
		LOG_ERR("exercise incomplete or sensor error: %d", rc);
		return rc;
	}
	LOG_INF("x=%d y=%d z=%d mm/s2", actual.x_mm_s2, actual.y_mm_s2,
		actual.z_mm_s2);
	return 0;
}

