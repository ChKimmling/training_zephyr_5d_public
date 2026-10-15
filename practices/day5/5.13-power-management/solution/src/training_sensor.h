/* SPDX-License-Identifier: Apache-2.0 */

#ifndef TRAINING_SENSOR_H_
#define TRAINING_SENSOR_H_

#include <stdint.h>
#include <zephyr/device.h>

const struct device *training_sensor_device(void);
int training_sensor_read(const struct device *dev, int32_t *value_milli);
uint32_t training_sensor_transition_count(const struct device *dev);

#endif /* TRAINING_SENSOR_H_ */
