#ifndef EVENT_MESSAGES_H
#define EVENT_MESSAGES_H

#include <stdbool.h>
#include <stdint.h>

enum event_command_type {
	EVENT_CMD_START,
	EVENT_CMD_STOP,
	EVENT_CMD_RESET,
	EVENT_CMD_INPUT,
	EVENT_CMD_PULSE,
	EVENT_CMD_BURST,
};

struct event_command {
	enum event_command_type type;
	uint32_t sequence;
	int32_t value;
	uint32_t count;
	uint32_t duration_ms;
	uint32_t interval_ms;
};

enum event_input_source {
	EVENT_SOURCE_SHELL,
	EVENT_SOURCE_EMULATED,
	EVENT_SOURCE_GPIO,
};

struct event_input {
	enum event_input_source source;
	uint32_t sequence;
	bool active;
	int64_t timestamp;
};

#endif
