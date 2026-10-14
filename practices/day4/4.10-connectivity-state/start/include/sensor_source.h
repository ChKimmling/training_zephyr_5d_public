#ifndef DAY4_SENSOR_SOURCE_H_
#define DAY4_SENSOR_SOURCE_H_

#include <stdbool.h>
#include <stdint.h>

struct motion_axes {
	int32_t x_mm_s2;
	int32_t y_mm_s2;
	int32_t z_mm_s2;
};

int sensor_source_init(void);
int sensor_source_set_sample(const struct motion_axes *axes);
int sensor_source_read(struct motion_axes *axes);
void sensor_source_fail_next_read(bool fail);

#endif

