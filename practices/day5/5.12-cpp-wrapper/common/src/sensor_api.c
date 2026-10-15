#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#include "sensor_api.h"

static bool resource_in_use;

int sensor_api_open(struct sensor_api_handle *handle, enum sensor_api_channel channel)
{
	if (handle == NULL ||
	    (channel != SENSOR_API_CHANNEL_ACCEL && channel != SENSOR_API_CHANNEL_GYRO)) {
		return -EINVAL;
	}

	if (resource_in_use) {
		return -EBUSY;
	}

	resource_in_use = true;
	handle->read_attempts = 0U;
	handle->channel = (uint8_t)channel;
	handle->opened = 1U;
	return 0;
}

int sensor_api_read(struct sensor_api_handle *handle, struct sensor_api_sample *sample)
{
	if (handle == NULL || sample == NULL || handle->opened == 0U) {
		return -EACCES;
	}

	handle->read_attempts++;
	if (handle->read_attempts == 2U) {
		return -EBUSY;
	}
	if (handle->read_attempts == 4U) {
		return -EIO;
	}

	sample->sequence = handle->read_attempts;
	sample->value_milli = (handle->channel == SENSOR_API_CHANNEL_ACCEL ? 1000 : 2000) +
			      (int32_t)(25U * handle->read_attempts);
	return 0;
}

void sensor_api_close(struct sensor_api_handle *handle)
{
	if (handle == NULL || handle->opened == 0U) {
		return;
	}

	handle->opened = 0U;
	resource_in_use = false;
}
