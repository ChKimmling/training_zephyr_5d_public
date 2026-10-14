#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(day4_check, LOG_LEVEL_INF);

#define MOTION_NODE DT_ALIAS(motion_sensor)

static int set_axis(const struct emul *emulator, enum sensor_channel channel, q31_t value)
{
	const struct sensor_chan_spec spec = {
		.chan_type = channel,
		.chan_idx = 0,
	};

	return emul_sensor_backend_set_channel(emulator, spec, &value, 3);
}

int main(void)
{
	const struct device *const sensor = DEVICE_DT_GET(MOTION_NODE);
	const struct emul *const emulator = EMUL_DT_GET(MOTION_NODE);
	struct sensor_value full_scale = { .val1 = 39, .val2 = 226600 };
	struct sensor_value values[3];
	int rc;

	if (!device_is_ready(sensor)) {
		LOG_ERR("motion sensor is not ready");
		return -ENODEV;
	}

	rc = sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ,
			     SENSOR_ATTR_FULL_SCALE, &full_scale);
	if (rc < 0) {
		LOG_ERR("cannot configure accelerometer: %d", rc);
		return rc;
	}

	rc = set_axis(emulator, SENSOR_CHAN_ACCEL_X, 0);
	rc = rc < 0 ? rc : set_axis(emulator, SENSOR_CHAN_ACCEL_Y, 1 << 28);
	rc = rc < 0 ? rc : set_axis(emulator, SENSOR_CHAN_ACCEL_Z, 2 << 28);
	if (rc < 0) {
		LOG_ERR("cannot configure emulator: %d", rc);
		return rc;
	}

	rc = sensor_sample_fetch(sensor);
	if (rc < 0) {
		LOG_ERR("sample fetch failed: %d", rc);
		return rc;
	}

	rc = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, values);
	if (rc < 0) {
		LOG_ERR("channel read failed: %d", rc);
		return rc;
	}

	LOG_INF("DAY4_SMOKE sensor=%s x=%lld y=%lld z=%lld mm/s2", sensor->name,
		(int64_t)sensor_value_to_milli(&values[0]),
		(int64_t)sensor_value_to_milli(&values[1]),
		(int64_t)sensor_value_to_milli(&values[2]));
	LOG_INF("DAY4_SMOKE PASS");
	return 0;
}

