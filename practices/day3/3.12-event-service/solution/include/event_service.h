#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#include "event_messages.h"

struct event_status {
	bool initialized;
	bool running;
	bool input_ready;
	bool input_active;
	const char *stimulus_state;
	uint32_t remaining;
	int last_error;
};

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
void event_service_set_running(bool running);
bool event_service_is_running(void);
void event_service_record_command_published(void);
void event_service_record_command_received(void);
void event_service_record_input_published(void);
void event_service_record_input_received(void);
void event_service_record_processed(void);
void event_service_record_publish_failure(int error);
uint32_t event_service_next_sequence(void);
void event_service_set_input_ready(bool ready);
void event_service_reject(void);
void event_service_set_error(int error);
int event_service_get_status(struct event_status *status);
int event_service_get_stats(struct event_stats *stats);
int event_service_reset_stats(void);

#endif
