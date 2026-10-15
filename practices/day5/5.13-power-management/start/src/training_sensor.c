/* SPDX-License-Identifier: Apache-2.0 */

#include "training_sensor.h"

#include <errno.h>
#include <stdbool.h>
#include <zephyr/init.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/printk.h>

struct training_sensor_data {
	bool active;
	uint32_t transitions;
	uint32_t samples;
};

static int training_sensor_pm_action(const struct device *dev, enum pm_device_action action)
{
	struct training_sensor_data *data = dev->data;

	/* TODO(5.13-01): SUSPEND und RESUME behandeln, sonst -ENOTSUP liefern. */
	ARG_UNUSED(action);
	ARG_UNUSED(data);

	return -ENOTSUP;
}

static int training_sensor_init(const struct device *dev)
{
	struct training_sensor_data *data = dev->data;

	data->active = true;
	data->transitions = 0U;
	data->samples = 0U;

	return 0;
}

PM_DEVICE_DEFINE(training_sensor_pm, training_sensor_pm_action);

static struct training_sensor_data training_sensor_data;

DEVICE_DEFINE(training_sensor, "training_sensor", training_sensor_init,
	      PM_DEVICE_GET(training_sensor_pm), &training_sensor_data, NULL, POST_KERNEL,
	      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, NULL);

const struct device *training_sensor_device(void)
{
	return DEVICE_GET(training_sensor);
}

int training_sensor_read(const struct device *dev, int32_t *value_milli)
{
	struct training_sensor_data *data = dev->data;

	if (value_milli == NULL) {
		return -EINVAL;
	}
	if (!data->active) {
		return -EACCES;
	}

	data->samples++;
	*value_milli = 20000 + (int32_t)(data->samples * 125U);

	return 0;
}

uint32_t training_sensor_transition_count(const struct device *dev)
{
	const struct training_sensor_data *data = dev->data;

	return data->transitions;
}
