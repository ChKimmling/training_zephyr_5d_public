#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdint.h>

struct event_stats {
	uint32_t raw_count;
	uint32_t processed_count;
	uint32_t rejected_count;
	uint32_t lost_count;
};

int event_service_init(void);
void event_service_record_input_event(void);
void event_service_get_stats(struct event_stats *stats);
int event_service_reset_stats(void);

#endif
