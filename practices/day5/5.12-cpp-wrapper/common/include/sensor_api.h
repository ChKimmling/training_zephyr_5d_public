#ifndef TRAINING_SENSOR_API_H_
#define TRAINING_SENSOR_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sensor_api_channel {
	SENSOR_API_CHANNEL_ACCEL = 0,
	SENSOR_API_CHANNEL_GYRO = 1,
};

struct sensor_api_handle {
	uint32_t read_attempts;
	uint8_t channel;
	uint8_t opened;
};

struct sensor_api_sample {
	int32_t value_milli;
	uint32_t sequence;
};

int sensor_api_open(struct sensor_api_handle *handle, enum sensor_api_channel channel);
int sensor_api_read(struct sensor_api_handle *handle, struct sensor_api_sample *sample);
void sensor_api_close(struct sensor_api_handle *handle);

#ifdef __cplusplus
}
#endif

#endif
