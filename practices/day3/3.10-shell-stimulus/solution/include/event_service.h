#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdint.h>

struct event_stats {
	uint32_t cmd_published;
	uint32_t cmd_received;
	uint32_t cmd_rejected;
	uint32_t input_published;
	uint32_t input_received;
	uint32_t processed;
	uint32_t publish_failed;
	int32_t last_error;
};

int event_service_init(void);
void event_service_record_command_published(void);
void event_service_record_command_received(void);
void event_service_reject(int error);
void event_service_record_input_published(void);
void event_service_record_input_received(void);
void event_service_record_publish_failure(int error);
int event_service_get_stats(struct event_stats *stats);
int event_service_reset_stats(void);

#endif
