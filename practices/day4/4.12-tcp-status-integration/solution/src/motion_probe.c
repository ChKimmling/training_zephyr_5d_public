#include "motion_probe.h"

#include <stdlib.h>
#include <string.h>

#define MOTION_RANGE_LIMIT_MM_S2 20000

void motion_probe_build(struct motion_probe *probe, const struct motion_axes *axes,
			int sensor_rc, uint32_t sequence, int64_t captured_ms)
{
	memset(probe, 0, sizeof(*probe));
	probe->sequence = sequence;
	probe->captured_ms = captured_ms;

	if (sensor_rc < 0 || axes == NULL) {
		probe->quality = MOTION_QUALITY_SENSOR_ERROR;
		return;
	}

	probe->x_mm_s2 = axes->x_mm_s2;
	probe->y_mm_s2 = axes->y_mm_s2;
	probe->z_mm_s2 = axes->z_mm_s2;
	if (abs(probe->x_mm_s2) > MOTION_RANGE_LIMIT_MM_S2 ||
	    abs(probe->y_mm_s2) > MOTION_RANGE_LIMIT_MM_S2 ||
	    abs(probe->z_mm_s2) > MOTION_RANGE_LIMIT_MM_S2) {
		probe->quality = MOTION_QUALITY_RANGE;
	} else {
		probe->quality = MOTION_QUALITY_OK;
	}
}

int64_t motion_probe_age_ms(const struct motion_probe *probe, int64_t now_ms)
{
	if (probe == NULL || now_ms <= probe->captured_ms) {
		return 0;
	}
	return now_ms - probe->captured_ms;
}

const char *motion_quality_name(enum motion_quality quality)
{
	switch (quality) {
	case MOTION_QUALITY_OK: return "OK";
	case MOTION_QUALITY_STALE: return "STALE";
	case MOTION_QUALITY_RANGE: return "RANGE";
	case MOTION_QUALITY_SENSOR_ERROR: return "SENSOR_ERROR";
	case MOTION_QUALITY_GAP: return "GAP";
	case MOTION_QUALITY_OVERLOAD: return "OVERLOAD";
	default: return "UNKNOWN";
	}
}

