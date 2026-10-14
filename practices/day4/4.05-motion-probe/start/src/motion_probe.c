#include "motion_probe.h"

#include <string.h>
#include <zephyr/sys/util.h>

void motion_probe_build(struct motion_probe *probe, const struct motion_axes *axes,
			int sensor_rc, uint32_t sequence, int64_t captured_ms)
{
	/* TODO(4.05-01): Die vollständige Probe deterministisch initialisieren. */
	memset(probe, 0, sizeof(*probe));
	/* TODO(4.05-02): Die übergebene Sequenznummer genau einmal übernehmen. */
	ARG_UNUSED(sequence);
	/* TODO(4.05-03): Den monotonen Erfassungszeitpunkt speichern. */
	ARG_UNUSED(captured_ms);
	/* TODO(4.05-04): Sensorfehler und den gültigen Bereich von ±20000 mm/s²
	 * prüfen und den passenden Qualitätsstatus setzen. */
	ARG_UNUSED(axes);
	ARG_UNUSED(sensor_rc);
}

int64_t motion_probe_age_ms(const struct motion_probe *probe, int64_t now_ms)
{
	/* TODO(4.05-05): Das Alter aus monotonen Zeitwerten berechnen und bei
	 * now_ms <= captured_ms den Wert 0 liefern. */
	ARG_UNUSED(probe);
	ARG_UNUSED(now_ms);
	return 0;
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
