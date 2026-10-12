#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

struct event_status {
	bool initialized;
	bool input_ready;
	bool input_active;
	const char *stimulus_state;
	int last_error;
};

struct event_stats {
	uint32_t raw_count;
	uint32_t processed_count;
	uint32_t rejected_count;
};

int event_service_init(void);
int event_service_get_status(struct event_status *status);
int event_service_get_stats(struct event_stats *stats);
int event_service_reset_stats(void);

#endif
