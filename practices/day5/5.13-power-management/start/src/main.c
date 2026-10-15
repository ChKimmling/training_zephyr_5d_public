/* SPDX-License-Identifier: Apache-2.0 */

#include "training_sensor.h"

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/device_runtime.h>
#include <zephyr/sys/printk.h>

static const char *state_name(enum pm_device_state state)
{
	switch (state) {
	case PM_DEVICE_STATE_ACTIVE:
		return "ACTIVE";
	case PM_DEVICE_STATE_SUSPENDED:
		return "SUSPENDED";
	case PM_DEVICE_STATE_SUSPENDING:
		return "SUSPENDING";
	case PM_DEVICE_STATE_OFF:
		return "OFF";
	default:
		return "UNKNOWN";
	}
}

static void print_snapshot(const char *label, const struct device *dev)
{
	enum pm_device_state state = PM_DEVICE_STATE_OFF;
	int ret = pm_device_state_get(dev, &state);

	printk("SNAPSHOT %-18s state=%s usage=%d busy=%d transitions=%u state_ret=%d\n", label,
	       state_name(state), pm_device_runtime_usage(dev), pm_device_is_busy(dev),
	       training_sensor_transition_count(dev), ret);
}

static int runtime_pm_enable(const struct device *dev)
{
	/* TODO(5.13-02): Runtime PM fuer das Geraet aktivieren. */
	ARG_UNUSED(dev);

	return -ENOSYS;
}

static int sensor_acquire(const struct device *dev)
{
	/* TODO(5.13-03): Eine Runtime-PM-Nutzungsreferenz beziehen. */
	ARG_UNUSED(dev);

	return -ENOSYS;
}

static int sensor_release(const struct device *dev)
{
	/* TODO(5.13-04): Die Runtime-PM-Nutzungsreferenz freigeben. */
	ARG_UNUSED(dev);

	return -ENOSYS;
}

static int critical_operation(const struct device *dev)
{
	int32_t value_milli = 0;
	int ret;

	/* TODO(5.13-05): Busy vor dem Zugriff setzen und auf jedem Pfad loeschen. */
	ret = training_sensor_read(dev, &value_milli);
	printk("CRITICAL busy=%d read_ret=%d value_milli=%d\n", pm_device_is_busy(dev), ret,
	       value_milli);

	return ret;
}

int main(void)
{
	const struct device *sensor = training_sensor_device();
	int32_t value_milli = 0;
	int ret;

	printk("POWER_MANAGEMENT device_ready=%d\n", device_is_ready(sensor));
	print_snapshot("after-init", sensor);

	ret = runtime_pm_enable(sensor);
	printk("STEP enable ret=%d\n", ret);
	print_snapshot("after-enable", sensor);

	ret = sensor_acquire(sensor);
	printk("STEP get-1 ret=%d\n", ret);
	print_snapshot("after-get-1", sensor);

	ret = training_sensor_read(sensor, &value_milli);
	printk("READ ret=%d value_milli=%d\n", ret, value_milli);

	ret = sensor_acquire(sensor);
	printk("STEP get-2 ret=%d\n", ret);
	print_snapshot("after-get-2", sensor);

	(void)critical_operation(sensor);
	print_snapshot("after-critical", sensor);

	ret = sensor_release(sensor);
	printk("STEP put-1 ret=%d\n", ret);
	print_snapshot("after-put-1", sensor);

	ret = sensor_release(sensor);
	printk("STEP put-2 ret=%d\n", ret);
	print_snapshot("after-put-2", sensor);

	ret = sensor_release(sensor);
	printk("STEP put-unbalanced ret=%d\n", ret);
	print_snapshot("final", sensor);

	return 0;
}
