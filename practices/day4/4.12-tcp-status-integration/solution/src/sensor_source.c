#include "sensor_source.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#if defined(CONFIG_BOARD_NATIVE_SIM)
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>
#endif

#if defined(CONFIG_BOARD_NATIVE_SIM)
#define MOTION_NODE DT_ALIAS(motion_sensor)
#define EMUL_SHIFT 4

static const struct device *const motion_sensor = DEVICE_DT_GET(MOTION_NODE);
static const struct emul *const motion_emulator = EMUL_DT_GET(MOTION_NODE);
static bool fail_next_read;

static q31_t milli_to_q31(int32_t milli)
{
	return (q31_t)(((int64_t)milli * (1LL << (31 - EMUL_SHIFT))) / 1000);
}

static int set_axis(enum sensor_channel channel, int32_t milli)
{
	const struct sensor_chan_spec spec = { .chan_type = channel, .chan_idx = 0 };
	const q31_t value = milli_to_q31(milli);

	return emul_sensor_backend_set_channel(motion_emulator, spec, &value, EMUL_SHIFT);
}

int sensor_source_init(void)
{
	struct sensor_value full_scale = { .val1 = 39, .val2 = 226600 };

	if (!device_is_ready(motion_sensor)) {
		return -ENODEV;
	}

	return sensor_attr_set(motion_sensor, SENSOR_CHAN_ACCEL_XYZ,
			       SENSOR_ATTR_FULL_SCALE, &full_scale);
}

int sensor_source_set_sample(const struct motion_axes *axes)
{
	int rc;

	if (axes == NULL) {
		return -EINVAL;
	}

	rc = set_axis(SENSOR_CHAN_ACCEL_X, axes->x_mm_s2);
	rc = rc < 0 ? rc : set_axis(SENSOR_CHAN_ACCEL_Y, axes->y_mm_s2);
	rc = rc < 0 ? rc : set_axis(SENSOR_CHAN_ACCEL_Z, axes->z_mm_s2);
	return rc;
}

int sensor_source_read(struct motion_axes *axes)
{
	struct sensor_value values[3];
	struct motion_axes next;
	int rc;

	if (axes == NULL) {
		return -EINVAL;
	}
	if (fail_next_read) {
		fail_next_read = false;
		return -EIO;
	}

	rc = sensor_sample_fetch(motion_sensor);
	if (rc < 0) {
		return rc;
	}
	rc = sensor_channel_get(motion_sensor, SENSOR_CHAN_ACCEL_XYZ, values);
	if (rc < 0) {
		return rc;
	}

	next.x_mm_s2 = (int32_t)sensor_value_to_milli(&values[0]);
	next.y_mm_s2 = (int32_t)sensor_value_to_milli(&values[1]);
	next.z_mm_s2 = (int32_t)sensor_value_to_milli(&values[2]);
	*axes = next;
	return 0;
}

void sensor_source_fail_next_read(bool fail)
{
	fail_next_read = fail;
}
#else
/* Das nRF7002 DK besitzt in der verwendeten Boarddefinition keinen
 * Bewegungssensor. Der optionale Hardwaretransfer nutzt deshalb dieselbe
 * Sensor-Schnittstelle mit einem kontrollierbaren synthetischen Wert. */
static struct motion_axes synthetic_sample = {0, 0, 9807};
static bool fail_next_read;

int sensor_source_init(void)
{
	return 0;
}

int sensor_source_set_sample(const struct motion_axes *axes)
{
	if (axes == NULL) {
		return -EINVAL;
	}
	synthetic_sample = *axes;
	return 0;
}

int sensor_source_read(struct motion_axes *axes)
{
	if (axes == NULL) {
		return -EINVAL;
	}
	if (fail_next_read) {
		fail_next_read = false;
		return -EIO;
	}
	*axes = synthetic_sample;
	return 0;
}

void sensor_source_fail_next_read(bool fail)
{
	fail_next_read = fail;
}
#endif

