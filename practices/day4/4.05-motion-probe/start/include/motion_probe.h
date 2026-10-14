#ifndef DAY4_MOTION_PROBE_H_
#define DAY4_MOTION_PROBE_H_

#include <stdint.h>

#include "sensor_source.h"

enum motion_quality {
	MOTION_QUALITY_OK,
	MOTION_QUALITY_STALE,
	MOTION_QUALITY_RANGE,
	MOTION_QUALITY_SENSOR_ERROR,
	MOTION_QUALITY_GAP,
	MOTION_QUALITY_OVERLOAD,
};

struct motion_probe {
	int32_t x_mm_s2;
	int32_t y_mm_s2;
	int32_t z_mm_s2;
	uint32_t sequence;
	int64_t captured_ms;
	enum motion_quality quality;
};

void motion_probe_build(struct motion_probe *probe, const struct motion_axes *axes,
			int sensor_rc, uint32_t sequence, int64_t captured_ms);
int64_t motion_probe_age_ms(const struct motion_probe *probe, int64_t now_ms);
const char *motion_quality_name(enum motion_quality quality);

#endif

