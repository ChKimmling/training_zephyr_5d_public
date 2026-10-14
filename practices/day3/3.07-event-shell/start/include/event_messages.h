#ifndef EVENT_MESSAGES_H
#define EVENT_MESSAGES_H

#include <stdint.h>

enum event_command_type {
	EVENT_CMD_START,
	EVENT_CMD_STOP,
	EVENT_CMD_RESET,
	EVENT_CMD_INPUT,
	EVENT_CMD_PULSE,
	EVENT_CMD_BURST,
	EVENT_CMD_CANCEL,
};

struct event_command {
	enum event_command_type type;
	uint32_t sequence;
	int32_t value;
	uint32_t count;
	uint32_t duration_ms;
	uint32_t interval_ms;
};

#endif
