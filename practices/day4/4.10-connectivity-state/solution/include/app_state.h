#ifndef DAY4_APP_STATE_H_
#define DAY4_APP_STATE_H_

#include <stdbool.h>
#include <stdint.h>

#include "motion_probe.h"

struct app_state_snapshot {
	bool has_probe;
	struct motion_probe last_probe;
	uint32_t processed;
	uint32_t sensor_errors;
	uint32_t queue_drops;
	uint32_t sequence_gaps;
	bool connected;
	uint32_t connect_transitions;
	uint32_t disconnect_transitions;
};

void app_state_reset(void);
void app_state_note_sensor_error(void);
void app_state_note_queue_drop(void);
void app_state_accept_probe(const struct motion_probe *probe);
void app_state_get(struct app_state_snapshot *snapshot);
void app_state_set_connected(bool connected);

#endif

