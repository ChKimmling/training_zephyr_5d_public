#include "sensor_source.h"

#include <errno.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>

#define MOTION_NODE DT_ALIAS(motion_sensor)
#define EMUL_SHIFT 4

static const struct device *motion_sensor;
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

	/* TODO(4.04-01): Sensor über MOTION_NODE beziehen, device_is_ready() prüfen
	 * und nur bei einem bereiten Gerät die Vollbereichseinstellung setzen. */
	ARG_UNUSED(full_scale);
	return -EAGAIN;
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
	struct motion_axes next = {0};
	int rc;

	if (axes == NULL) {
		return -EINVAL;
	}
	if (fail_next_read) {
		fail_next_read = false;
		return -EIO;
	}
	/* TODO(4.04-02): sensor_sample_fetch() aufrufen und Fehler unverändert
	 * zurückgeben. */
	rc = -EAGAIN;
	if (rc < 0) {
		return rc;
	}
	/* TODO(4.04-03): SENSOR_CHAN_ACCEL_XYZ lesen und den Rückgabewert
	 * prüfen, bevor values verwendet wird. */
	/* TODO(4.04-04): Alle drei sensor_value-Werte in mm/s² umrechnen. */
	ARG_UNUSED(values);
	/* TODO(4.04-05): Erst nach vollständig erfolgreichem Lesen die lokale
	 * next-Struktur nach *axes kopieren. */
	ARG_UNUSED(next);
	return -EAGAIN;
}

void sensor_source_fail_next_read(bool fail)
{
	fail_next_read = fail;
}
