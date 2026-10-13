#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

struct event_status {
	bool initialized;
	bool input_ready;
	bool input_active;
	const char *stimulus_state;
	uint32_t remaining;
	int last_error;
};

struct event_stats {
	uint32_t raw_count;
	uint32_t processed_count;
	uint32_t rejected_count;
	uint32_t lost_count;
};

int event_service_init(void);
void event_service_set_input_ready(bool ready);
void event_service_record_input_event(void);
void event_service_reject(void);
void event_service_set_error(int error);
int event_service_get_status(struct event_status *status);
int event_service_get_stats(struct event_stats *stats);
int event_service_reset_stats(void);

#endif
